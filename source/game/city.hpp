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
//
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com


#ifndef __OPENCAESAR3_CITY_H_INCLUDED__
#define __OPENCAESAR3_CITY_H_INCLUDED__

#include "walker/walker.hpp"
#include "enums.hpp"
#include "core/serializer.hpp"
#include "core/signals.hpp"
#include "core/predefinitions.hpp"
#include "core/referencecounted.hpp"
#include "game/cityservice.hpp"
#include "gfx/tile.hpp"
#include "empire_city.hpp"
#include "core/position.hpp"
#include "core/foreach.hpp"
#include "game/player.hpp"
#include "building/constants.hpp"

class DateTime;
class CityBuildOptions;
class CityTradeOptions;
class CityWinTargets;
class CityFunds;

struct BorderInfo
{
  TilePos roadEntry;
  TilePos roadExit;
  TilePos boatEntry;
  TilePos boatExit;
};

class City : public EmpireCity
{
public:
  static CityPtr create(EmpirePtr empire, PlayerPtr player );
  ~City();

  virtual void timeStep( unsigned int time );  // performs one simulation step

  void setLocation( const Point& location );
  Point getLocation() const;

  WalkerList getWalkers( constants::walker::Type type );
  WalkerList getWalkers( constants::walker::Type type, TilePos startPos, TilePos stopPos=TilePos( -1, -1 ) );
  void addWalker( WalkerPtr walker );
  void removeWalker( WalkerPtr walker );

  void addService( CityServicePtr service );
  CityServicePtr findService( const std::string& name ) const;

  TileOverlayList& getOverlays();

  void setBorderInfo( const BorderInfo& info );
  const BorderInfo& getBorderInfo() const;

  PlayerPtr getPlayer() const;
  
  void setCameraPos(const TilePos pos);
  TilePos getCameraPos() const;
     
  ClimateType getClimate() const;
  void setClimate(const ClimateType);

  CityFunds& getFunds() const;

  int getPopulation() const;
  int getProsperity() const;
  int getCulture() const;

  Tilemap& getTilemap();

  std::string getName() const; 
  void setName( const std::string& name );

  void save( VariantMap& stream) const;
  void load( const VariantMap& stream);

  // add construction
  void addOverlay(TileOverlayPtr overlay);
  TileOverlayPtr getOverlay( const TilePos& pos ) const;

  const CityBuildOptions& getBuildOptions() const;

  void setBuildOptions( const CityBuildOptions& options );

  const CityWinTargets& getWinTargets() const;
  void setWinTargets( const CityWinTargets& targets );

  CityTradeOptions& getTradeOptions();

  void resolveMerchantArrived( EmpireMerchantPtr merchant );

  virtual const GoodStore& getSells() const;
  virtual const GoodStore& getBuys() const;

  virtual EmpirePtr getEmpire() const;

  void updateRoads();
   
oc3_signals public:
  Signal1<int>& onPopulationChanged();
  Signal1<int>& onFundsChanged();
  Signal1<std::string>& onWarningMessage();
  Signal2<TilePos,std::string>& onDisasterEvent();

protected:
  void monthStep( const DateTime& time );

private:
  City();

  class Impl;
  ScopedPtr< Impl > _d;
};

class CityHelper
{
public:
  CityHelper( CityPtr city ) : _city( city ) {}

  template< class T >
  std::list< SmartPtr< T > > find( const TileOverlay::Type type )
  {
    std::list< SmartPtr< T > > ret;
    TileOverlayList& buildings = _city->getOverlays();
    foreach( TileOverlayPtr item, buildings )
    {
      SmartPtr< T > b = item.as<T>();
      if( b.isValid() && (b->getType() == type || type == constants::building::any ) )
      {
        ret.push_back( b );
      }
    }

    return ret;
  }

  template< class T >
  std::list< SmartPtr< T > > find( constants::building::Group group )
  {
    std::list< SmartPtr< T > > ret;
    TileOverlayList& buildings = _city->getOverlays();
    foreach( TileOverlayPtr item, buildings )
    {
      SmartPtr< T > b = item.as<T>();
      if( b.isValid() && (b->getClass() == group || group == constants::building::anyGroup ) )
      {
        ret.push_back( b );
      }
    }

    return ret;
  }


  template< class T >
  SmartPtr< T > find( const TileOverlay::Type type, const TilePos& pos )
  {   
    TileOverlayPtr overlay = _city->getOverlay( pos );
    if( overlay->getType() == type || type == constants::building::any )
    {
      return overlay.as< T >();
    }

    return SmartPtr<T>();
  }

  template< class T >
  std::list< SmartPtr< T > > find( constants::walker::Type type,
                                   TilePos start, TilePos stop=TilePos(-1,-1) )
  {
    std::list< SmartPtr< T > > ret;

    WalkerList walkers = _city->getWalkers( type, start, stop );
    foreach( WalkerPtr w, walkers )
    {
      if( w.is<T>() )
      {
        ret.push_back( w.as<T>() );
      }
    }

    return ret;
  }

  template< class T >
  std::list< SmartPtr< T > > find( const TileOverlay::Type type, TilePos start, TilePos stop )
  {
    std::set< SmartPtr< T > > tmp;

    TilemapArea area = getArea( start, stop );
    foreach( Tile* tile, area )
    {
      SmartPtr<T> obj = tile->getOverlay().as<T>();
      if( obj.isValid() && (obj->getType() == type || type == constants::building::any) )
      {
        tmp.insert( obj );
      }
    }    

    std::list< SmartPtr< T > > ret;
    foreach( SmartPtr<T> obj, tmp )
    {
      ret.push_back( obj );
    }

    return ret;
  }

  template< class T >
  std::list< SmartPtr< T > > find( constants::building::Group group, TilePos start, TilePos stop )
  {
    std::set< SmartPtr< T > > tmp;

    TilemapArea area = getArea( start, stop );

    foreach( Tile* tile, area )
    {
      SmartPtr<T> obj = tile->getOverlay().as<T>();
      if( obj.isValid() && (obj->getClass() == group || group == constants::building::anyGroup) )
      {
        tmp.insert( obj );
      }
    }

    std::list< SmartPtr< T > > ret;
    foreach( SmartPtr<T> obj, tmp )
    {
      ret.push_back( obj );
    }

    return ret;
  }

  template< class T >
  std::list< SmartPtr< T > > getProducers( const Good::Type goodtype )
  {
    std::list< SmartPtr< T > > ret;
    TileOverlayList& overlays = _city->getOverlays();
    foreach( TileOverlayPtr item, overlays )
    {
      SmartPtr< T > b = item.as<T>();
      if( b.isValid() && b->getOutGoodType() == goodtype )
      {
        ret.push_back( b );
      }
    }

    return ret;
  }

  TilemapArea getArea( TileOverlayPtr overlay );
  TilemapArea getArea( TilePos start, TilePos stop );

  void updateDesirability( ConstructionPtr construction, bool onBuild );

protected:
  CityPtr _city;
};

#endif //__OPENCAESAR3_CITY_H_INCLUDED__
