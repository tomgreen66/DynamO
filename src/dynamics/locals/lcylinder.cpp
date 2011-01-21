/*  DYNAMO:- Event driven molecular dynamics simulator 
    http://www.marcusbannerman.co.uk/dynamo
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

#include "lcylinder.hpp"
#include "../liouvillean/liouvillean.hpp"
#include "localEvent.hpp"
#include "../NparticleEventData.hpp"
#include "../overlapFunc/CubePlane.hpp"
#include "../units/units.hpp"
#include "../../datatypes/vector.xml.hpp"
#include "../../schedulers/scheduler.hpp"


CLCylinder::CLCylinder(DYNAMO::SimData* nSim, double ne, Vector  nnorm, 
		       Vector  norigin, double nr, std::string nname, 
		       CRange* nRange, bool nrender):
  Local(nRange, nSim, "CylinderWall"),
  vNorm(nnorm),
  vPosition(norigin),
  e(ne),
  radius(nr),
  render(nrender)
{
  localName = nname;
}

CLCylinder::CLCylinder(const XMLNode& XML, DYNAMO::SimData* tmp):
  Local(tmp, "CylinderWall")
{
  operator<<(XML);
}

LocalEvent 
CLCylinder::getEvent(const Particle& part) const
{
#ifdef ISSS_DEBUG
  if (!Sim->dynamics.getLiouvillean().isUpToDate(part))
    M_throw() << "Particle is not up to date";
#endif

  return LocalEvent(part, Sim->dynamics.getLiouvillean().getCylinderWallCollision
		     (part, vPosition, vNorm, radius), WALL, *this);
}

void
CLCylinder::runEvent(const Particle& part, const LocalEvent& iEvent) const
{
  ++Sim->eventCount;

  //Run the collision and catch the data
  NEventData EDat(Sim->dynamics.getLiouvillean().runCylinderWallCollision
		      (part, vPosition, vNorm, e));

  Sim->signalParticleUpdate(EDat);

  //Now we're past the event update the scheduler and plugins
  Sim->ptrScheduler->fullUpdate(part);
  
  BOOST_FOREACH(magnet::ClonePtr<OutputPlugin> & Ptr, Sim->outputPlugins)
    Ptr->eventUpdate(iEvent, EDat);
}

bool 
CLCylinder::isInCell(const Vector & Origin, const Vector& CellDim) const
{
  return true;
  //DYNAMO::OverlapFunctions::CubePlane
  //(Origin, CellDim, vPosition, vNorm);
}

void 
CLCylinder::initialise(size_t nID)
{
  ID = nID;
}

void 
CLCylinder::operator<<(const XMLNode& XML)
{
  range.set_ptr(CRange::loadClass(XML,Sim));
  
  try {
    e = boost::lexical_cast<double>(XML.getAttribute("Elasticity"));

    radius = boost::lexical_cast<double>(XML.getAttribute("Radius"))
      * Sim->dynamics.units().unitLength();

    render = boost::lexical_cast<bool>(XML.getAttribute("Render"));
    XMLNode xBrowseNode = XML.getChildNode("Norm");
    localName = XML.getAttribute("Name");
    vNorm << xBrowseNode;
    vNorm /= vNorm.nrm();
    xBrowseNode = XML.getChildNode("Origin");
    vPosition << xBrowseNode;
    vPosition *= Sim->dynamics.units().unitLength();
  } 
  catch (boost::bad_lexical_cast &)
    {
      M_throw() << "Failed a lexical cast in CLCylinder";
    }
}

void 
CLCylinder::outputXML(xml::XmlStream& XML) const
{
  XML << xml::attr("Type") << "CylinderWall" 
      << xml::attr("Name") << localName
      << xml::attr("Elasticity") << e
      << xml::attr("Radius") << radius / Sim->dynamics.units().unitLength()
      << xml::attr("Render") << render
      << range
      << xml::tag("Norm")
      << vNorm
      << xml::endtag("Norm")
      << xml::tag("Origin")
      << vPosition / Sim->dynamics.units().unitLength()
      << xml::endtag("Origin");
}

void 
CLCylinder::write_povray_info(std::ostream& os) const
{
  if (render)
    os << "intersection { cylinder { <0, -0.5, 0>, <0, 0.5, 0>," 
       << radius << " "
       << "Point_At_Trans(<"
       << vNorm[0] << "," << vNorm[1] << "," << vNorm[2] << ">)"
       << " translate <" << vPosition[0] << "," << vPosition[1] << "," << vPosition[2] << "> }"
       << "box { <" 
       << -Sim->aspectRatio[0]/2 - Sim->dynamics.units().unitLength() 
       << "," << -Sim->aspectRatio[1]/2 - Sim->dynamics.units().unitLength()  
       << "," << -Sim->aspectRatio[2]/2 - Sim->dynamics.units().unitLength() 
       << ">,"
       << "<" << Sim->aspectRatio[0]/2 + Sim->dynamics.units().unitLength()
       << "," << Sim->aspectRatio[1]/2 + Sim->dynamics.units().unitLength()
       << "," << Sim->aspectRatio[2]/2 + Sim->dynamics.units().unitLength()
       << "> }\n"
       << "pigment { Col_Glass_Bluish } }";
}
