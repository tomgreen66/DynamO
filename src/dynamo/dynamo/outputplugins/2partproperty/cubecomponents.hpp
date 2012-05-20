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
#include <dynamo/outputplugins/outputplugin.hpp>
#include <magnet/math/histogram.hpp>
#include <dynamo/outputplugins/eventtypetracking.hpp>
#include <map>

namespace dynamo {
  using namespace EventTypeTracking;

  class OPCubeComp: public OutputPlugin
  {
  public:
    OPCubeComp(const dynamo::SimData*, const magnet::xml::Node&);

    virtual void initialise();
  
    virtual void eventUpdate(const IntEvent&, const PairEventData&);

    virtual void eventUpdate(const GlobalEvent&, const NEventData&);

    virtual void eventUpdate(const LocalEvent&, const NEventData&);

    virtual void eventUpdate(const System&, const NEventData&, const double&);

    void output(magnet::xml::XmlStream &);

    virtual void changeSystem(OutputPlugin* plug) { std::swap(Sim, static_cast<OPCubeComp*>(plug)->Sim); }
  
  protected:
    struct mapdata
    {
      mapdata() 
      {
	angles[0] = magnet::math::Histogram<>(0.01);
	angles[1] = magnet::math::Histogram<>(0.01);
	angles[2] = magnet::math::Histogram<>(0.01);
      }
    
      magnet::math::Histogram<> angles[3];
    };
  
    typedef std::pair<EEventType, classKey> mapKey;

    std::map<mapKey, mapdata> angles;
  };
}
