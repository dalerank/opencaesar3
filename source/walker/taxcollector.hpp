// This file is part of openCaesar3.
//
// openCaesar3 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// openCaesar3 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with openCaesar3.  If not, see <http://www.gnu.org/licenses/>.

#ifndef __OPENCAESAR3_TAXCOLLECTOR_H_INCLUDED__
#define __OPENCAESAR3_TAXCOLLECTOR_H_INCLUDED__

#include "serviceman.hpp"

class TaxCollector;
typedef SmartPtr< TaxCollector > TaxCollectorPtr;

class TaxCollector : public ServiceWalker
{
public:
  static TaxCollectorPtr create( CityPtr city );
  virtual void onMidTile();

  int getMoney() const;

  virtual void onDestination();
  virtual void load(const VariantMap &stream);
  virtual void save(VariantMap &stream) const;

private:
  TaxCollector( CityPtr city );

  class Impl;
  ScopedPtr< Impl > _d;
};

#endif //__OPENCAESAR3_TAXCOLLECTOR_H_INCLUDED__
