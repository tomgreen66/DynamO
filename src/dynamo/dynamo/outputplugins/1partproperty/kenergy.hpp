/*  dynamo:- Event driven molecular dynamics simulator 
    http://www.dynamomd.org
    Copyright (C) 2011  Marcus N Campbell Bannerman <m.bannerman@gmail.com>

    This program is free software: you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    version 3 as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once
#include <dynamo/outputplugins/1partproperty/1partproperty.hpp>

namespace dynamo {
  class OPKEnergy: public OP1PP
  {
  public:
    OPKEnergy(const dynamo::SimData*, const magnet::xml::Node&);

    void A1ParticleChange(const ParticleEventData&);

    void A2ParticleChange(const PairEventData&);

    void stream(const double&);  

    void output(magnet::xml::XmlStream &); 

    void periodicOutput();

    virtual void initialise();

    double getAvgkT() const;

    double getAvgTheta() const;
     
    double getAvgSqTheta() const;
  
    void changeSystem(OutputPlugin*);

    void temperatureRescale(const double&);
  
    const double& getCurrentkT() const { return KECurrent; }

  protected:

    double InitialKE;
    double KEacc;
    double KEsqAcc;
    double KECurrent;  
  };
}
