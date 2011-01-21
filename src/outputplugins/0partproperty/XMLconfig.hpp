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

#ifndef OPCONFIG_H
#define OPCONFIG_H

#include "../outputplugin.hpp"

class OPConfig: public OutputPlugin
{
 public:
  OPConfig(const DYNAMO::SimData*);
  ~OPConfig();
  
  virtual void initialise() {}

  virtual void eventUpdate(const IntEvent&, const PairEventData&) {}

  virtual void eventUpdate(const GlobalEvent&, const NEventData&) {}

  virtual void eventUpdate(const LocalEvent&, const NEventData&) {}

  virtual void eventUpdate(const System&, const NEventData&, 
			   const double&) {}

  virtual void output(xml::XmlStream &); 
  
  virtual OutputPlugin *Clone() const { return new OPConfig(*this); };

  void fileOutput(const char *);

  void setRounding();

  void setUncompressed() { compressedOutput = false; }

private:
  bool rounding;
  bool compressedOutput;
};

#endif
