/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     | Website:  https://openfoam.org
    \\  /    A nd           | Copyright (C) 2011-2024 OpenFOAM Foundation
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

\*---------------------------------------------------------------------------*/

#include "axialAngularSpring.H"
#include "addToRunTimeSelectionTable.H"
#include "sixDoFRigidBodyMotion.H"
#include "transform.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
namespace sixDoFRigidBodyMotionRestraints
{
    defineTypeNameAndDebug(axialAngularSpring, 0);

    addToRunTimeSelectionTable
    (
        sixDoFRigidBodyMotionRestraint,
        axialAngularSpring,
        dictionary
    );
}
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::sixDoFRigidBodyMotionRestraints::axialAngularSpring::axialAngularSpring
(
    const word& name,
    const dictionary& sDoFRBMRDict
)
:
    sixDoFRigidBodyMotionRestraint(name, sDoFRBMRDict),
    refQ_(),
    axis_(),
    moment_(),
    damping_()
{
    read(sDoFRBMRDict);
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::sixDoFRigidBodyMotionRestraints::axialAngularSpring::~axialAngularSpring()
{}


// * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * * //

void Foam::sixDoFRigidBodyMotionRestraints::axialAngularSpring::restrain
(
    const sixDoFRigidBodyMotion& motion,
    vector& restraintPosition,
    vector& restraintForce,
    vector& restraintMoment
) const
{
    vector refDir = rotationTensor(vector(1, 0 ,0), axis_) & vector(0, 1, 0);

    vector oldDir = refQ_ & refDir;

    vector newDir = motion.orientation() & refDir;

    if (mag(oldDir & axis_) > 0.95 || mag(newDir & axis_) > 0.95)
    {
        // Directions getting close to the axis, change reference

        refDir = rotationTensor(vector(1, 0 ,0), axis_) & vector(0, 0, 1);
        oldDir = refQ_ & refDir;
        newDir = motion.orientation() & refDir;
    }

    // Removing any axis component from oldDir and newDir and normalising
    oldDir -= (axis_ & oldDir)*axis_;
    oldDir /= (mag(oldDir) + vSmall);

    newDir -= (axis_ & newDir)*axis_;
    newDir /= (mag(newDir) + vSmall);

    scalar theta = mag(acos(min(oldDir & newDir, 1.0)));

    // Determining the sign of theta by comparing the cross product of
    // the direction vectors with the axis
    theta *= sign((oldDir ^ newDir) & axis_);

    const scalar moment = moment_->value(theta);

    // Damping of along axis angular velocity only
    restraintMoment = moment*axis_ - damping_*(motion.omega() & axis_)*axis_;

    restraintForce = Zero;

    // Not needed to be altered as restraintForce is zero, but set to
    // centreOfRotation to be sure of no spurious moment
    restraintPosition = motion.centreOfRotation();

    if (motion.report())
    {
        Info<< " angle " << theta
            << " moment " << restraintMoment
            << endl;
    }
}


bool Foam::sixDoFRigidBodyMotionRestraints::axialAngularSpring::read
(
    const dictionary& sDoFRBMRDict
)
{
    sixDoFRigidBodyMotionRestraint::read(sDoFRBMRDict);

    refQ_ = sDoFRBMRCoeffs_.lookupOrDefault<tensor>("referenceOrientation", I);

    if (mag(mag(refQ_) - sqrt(3.0)) > 1e-9)
    {
        FatalErrorInFunction
            << "referenceOrientation " << refQ_ << " is not a rotation tensor. "
            << "mag(referenceOrientation) - sqrt(3) = "
            << mag(refQ_) - sqrt(3.0) << nl
            << exit(FatalError);
    }

    axis_ = sDoFRBMRCoeffs_.lookup("axis");

    scalar magAxis(mag(axis_));

    if (magAxis > vSmall)
    {
        axis_ /= magAxis;
    }
    else
    {
        FatalErrorInFunction
            << "axis has zero length"
            << abort(FatalError);
    }

    moment_ =
        Function1<scalar>::New
        (
            "moment",
            unitRadians,
            dimForce/dimLength,
            sDoFRBMRCoeffs_
        );

    if
    (
        sDoFRBMRCoeffs_.found("angleUnits")
     || sDoFRBMRCoeffs_.found("angleFormat")
    )
    {
        FatalIOErrorInFunction(sDoFRBMRCoeffs_)
            << "Angle units are no longer specified with 'angleUnits' or "
            << "'angleFormat' entries. Instead, parameters of the 'moment' "
            << "function can have their units specified directly."
            << exit(FatalIOError);
    }

    sDoFRBMRCoeffs_.lookup("damping") >> damping_;

    return true;
}


void Foam::sixDoFRigidBodyMotionRestraints::axialAngularSpring::write
(
    Ostream& os
) const
{
    writeEntry(os, "referenceOrientation", refQ_);

    writeEntry(os, "axis", axis_);

    writeEntry(os, moment_());

    writeEntry(os, "damping", damping_);
}


// ************************************************************************* //
