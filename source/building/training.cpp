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

#include "training.hpp"

#include "walker/trainee.hpp"
#include "core/exception.hpp"
#include "gui/info_box.hpp"
#include "core/gettext.hpp"
#include "core/variant.hpp"
#include "game/resourcegroup.hpp"
#include "game/city.hpp"
#include "constants.hpp"

using namespace constants;

TrainingBuilding::TrainingBuilding(const Type type, const Size& size )
  : WorkingBuilding( type, size )
{
   _trainingTimer = 0;
   _trainingDelay = 80;
}

void TrainingBuilding::timeStep(const unsigned long time)
{
   WorkingBuilding::timeStep(time);

   if( getWorkersCount() <= 0 )
   {
     return;
   }

   if( _trainingTimer == 0 )
   {
      deliverTrainee();
      _trainingTimer = _trainingDelay;
   }
   else if (_trainingTimer > 0)
   {
      _trainingTimer -= 1;
   }

   _animationRef().update( time );
   const Picture& pic = _animationRef().getFrame();
   if( pic.isValid() )
   {
      _fgPicturesRef().back() = _animationRef().getFrame();
   }
}

void TrainingBuilding::save( VariantMap& stream) const
{
  WorkingBuilding::save( stream );
  stream[ "trainingTimer" ] = _trainingTimer;
  stream[ "trainingDelay" ] = _trainingDelay;
}

void TrainingBuilding::load( const VariantMap& stream )
{
  WorkingBuilding::load( stream );
  _trainingTimer = (int)stream.get( "trainingTimer", 0 );
  _trainingDelay = (int)stream.get( "trainingDelay", 80 );
}

ActorColony::ActorColony() : TrainingBuilding( building::actorColony, Size(3) )
{
  //_getAnimation().load( ResourceGroup::entertaiment, 82, 9);
  //_getAnimation().setOffset( Point( 68, -6 ) );
  _fgPicturesRef().resize(1);
}

void ActorColony::deliverTrainee()
{
  if( !getWalkers().empty() )
  {
    return;
  }

  TraineeWalkerPtr trainee = TraineeWalker::create( _getCity(), walker::actor );
  trainee->setOriginBuilding(*this);
  trainee->send2City();

  if( !trainee->isDeleted() )
  {
    addWalker( trainee.as<Walker>() );
  }
}

GladiatorSchool::GladiatorSchool() : TrainingBuilding( building::gladiatorSchool, Size(3))
{
  //setPicture( Picture::load( ResourceGroup::entertaiment, 51 ) );

  _animationRef().load( ResourceGroup::entertaiment, 52, 10);
  _animationRef().setOffset( Point( 62, 24 ) );
  _fgPicturesRef().resize(1);
}

void GladiatorSchool::deliverTrainee()
{
   // std::cout << "Deliver trainee!" << std::endl;
  TraineeWalkerPtr trainee = TraineeWalker::create( _getCity(), walker::gladiator );
  trainee->setOriginBuilding(*this);
  trainee->send2City();
}


LionsNursery::LionsNursery() : TrainingBuilding( building::lionHouse, Size(3) )
{
  setPicture( ResourceGroup::entertaiment, 62 );

   _animationRef().load( ResourceGroup::entertaiment, 63, 18);
   _animationRef().setOffset( Point( 78, 21) );
   _fgPicturesRef().resize(1);
}

void LionsNursery::deliverTrainee()
{
  // std::cout << "Deliver trainee!" << std::endl;
  TraineeWalkerPtr trainee = TraineeWalker::create( _getCity(), walker::tamer );
  trainee->setOriginBuilding(*this);
  trainee->send2City();
}


WorkshopChariot::WorkshopChariot() : TrainingBuilding( building::chariotSchool, Size(3) )
{
  setPicture( ResourceGroup::entertaiment, 91 );

  _animationRef().load( ResourceGroup::entertaiment, 92, 10);
  _animationRef().setOffset( Point( 54, 23 ));
  _fgPicturesRef().resize(1);
}

void WorkshopChariot::deliverTrainee()
{
   // std::cout << "Deliver trainee!" << std::endl;
  TraineeWalkerPtr trainee = TraineeWalker::create( _getCity(), walker::charioter );
  trainee->setOriginBuilding(*this);
  trainee->send2City();
}


