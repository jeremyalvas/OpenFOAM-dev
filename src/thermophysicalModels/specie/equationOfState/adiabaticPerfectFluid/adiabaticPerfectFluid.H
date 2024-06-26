/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     | Website:  https://openfoam.org
    \\  /    A nd           | Copyright (C) 2013-2023 OpenFOAM Foundation
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

Class
    Foam::adiabaticPerfectFluid

Description
    Adiabatic perfect fluid equation of state for liquids:

    \verbatim
        rho = rho0*pow((p + B)/(p0 + B), 1/gamma)
    \endverbatim

    Equivalent to the Murnaghan-Tait equation of state with different
    coefficient specification.

    Coefficient mixing is very inaccurate and not supported,
    so this equation of state is not applicable to mixtures.

Usage
    \table
        Property     | Description
        rho0         | Reference density
        R            | Fluid constant
    \endtable

    Example specification of the adiabaticPerfectFluid equation of state
    for LNG:
    \verbatim
    equationOfState
    {
        rho0            450;
        p0              1e5;
        gamma           15.4;
        B               50000000
    }
    \endverbatim

SourceFiles
    adiabaticPerfectFluidI.H
    adiabaticPerfectFluid.C

\*---------------------------------------------------------------------------*/

#ifndef adiabaticPerfectFluid_H
#define adiabaticPerfectFluid_H

#include "autoPtr.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// Forward declaration of friend functions and operators

template<class Specie> class adiabaticPerfectFluid;

template<class Specie>
inline adiabaticPerfectFluid<Specie> operator+
(
    const adiabaticPerfectFluid<Specie>&,
    const adiabaticPerfectFluid<Specie>&
);

template<class Specie>
inline adiabaticPerfectFluid<Specie> operator*
(
    const scalar,
    const adiabaticPerfectFluid<Specie>&
);

template<class Specie>
inline adiabaticPerfectFluid<Specie> operator==
(
    const adiabaticPerfectFluid<Specie>&,
    const adiabaticPerfectFluid<Specie>&
);

template<class Specie>
Ostream& operator<<
(
    Ostream&,
    const adiabaticPerfectFluid<Specie>&
);


/*---------------------------------------------------------------------------*\
                    Class adiabaticPerfectFluid Declaration
\*---------------------------------------------------------------------------*/

template<class Specie>
class adiabaticPerfectFluid
:
    public Specie
{
    // Private Data

        //- Reference pressure
        scalar p0_;

        //- Reference density
        scalar rho0_;

        //- The isentropic exponent
        scalar gamma_;

        //- Pressure offset for a stiffened gas
        scalar B_;


public:

    // Constructors

        //- Construct from components
        inline adiabaticPerfectFluid
        (
            const Specie& sp,
            const scalar p0,
            const scalar rho0,
            const scalar gamma,
            const scalar B
        );

        //- Construct from name and dictionary
        adiabaticPerfectFluid(const word& name, const dictionary& dict);

        //- Construct as named copy
        inline adiabaticPerfectFluid
        (
            const word& name,
            const adiabaticPerfectFluid&
        );

        //- Construct and return a clone
        inline autoPtr<adiabaticPerfectFluid> clone() const;


    // Member Functions

        //- Return the instantiated type name
        static word typeName()
        {
            return "adiabaticPerfectFluid<" + word(Specie::typeName_()) + '>';
        }


        // Fundamental properties

            //- Is the equation of state is incompressible i.e. rho != f(p)
            static const bool incompressible = false;

            //- Is the equation of state is isochoric i.e. rho = const
            static const bool isochoric = false;

            //- Return density [kg/m^3]
            inline scalar rho(scalar p, scalar T) const;

            //- Return enthalpy contribution [J/kg]
            inline scalar h(const scalar p, const scalar T) const;

            //- Return Cp contribution [J/(kg K]
            inline scalar Cp(scalar p, scalar T) const;

            //- Return internal energy contribution [J/kg]
            inline scalar e(const scalar p, const scalar T) const;

            //- Return Cv contribution [J/(kg K]
            inline scalar Cv(scalar p, scalar T) const;

            //- Return entropy contribution to the integral of Cp/T [J/kg/K]
            inline scalar sp(const scalar p, const scalar T) const;

            //- Return entropy contribution to the integral of Cv/T [J/kg/K]
            inline scalar sv(const scalar p, const scalar T) const;

            //- Return compressibility [s^2/m^2]
            inline scalar psi(scalar p, scalar T) const;

            //- Return compression factor []
            inline scalar Z(scalar p, scalar T) const;

            //- Return (Cp - Cv) [J/(kg K]
            inline scalar CpMCv(scalar p, scalar T) const;

            //- Return volumetric coefficient of thermal expansion [1/T]
            inline scalar alphav(const scalar p, const scalar T) const;


        // IO

            //- Write to Ostream
            void write(Ostream& os) const;


    // Member Operators

        inline void operator+=(const adiabaticPerfectFluid&);
        inline void operator*=(const scalar);


    // Friend operators

        friend adiabaticPerfectFluid operator+ <Specie>
        (
            const adiabaticPerfectFluid&,
            const adiabaticPerfectFluid&
        );

        friend adiabaticPerfectFluid operator* <Specie>
        (
            const scalar s,
            const adiabaticPerfectFluid&
        );

        friend adiabaticPerfectFluid operator== <Specie>
        (
            const adiabaticPerfectFluid&,
            const adiabaticPerfectFluid&
        );


    // Ostream Operator

        friend Ostream& operator<< <Specie>
        (
            Ostream&,
            const adiabaticPerfectFluid&
        );
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "adiabaticPerfectFluidI.H"

#ifdef NoRepository
    #include "adiabaticPerfectFluid.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
