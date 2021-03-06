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

#include "advisor_religion_window.hpp"
#include "gfx/picture.hpp"
#include "gfx/decorator.hpp"
#include "core/gettext.hpp"
#include "pushbutton.hpp"
#include "label.hpp"
#include "game/resourcegroup.hpp"
#include "core/stringhelper.hpp"
#include "gfx/engine.hpp"
#include "core/gettext.hpp"
#include "game/enums.hpp"
#include "game/city.hpp"
#include "building/house.hpp"
#include "texturedbutton.hpp"
#include "game/divinity.hpp"
#include "game/gamedate.hpp"
#include "building/constants.hpp"

using namespace constants;

namespace gui
{

class ReligionInfoLabel : public Label
{
public:
  ReligionInfoLabel( Widget* parent, const Rect& rect, RomeDivinityPtr divinity, 
                     int smallTempleCount, int bigTempleCount  )
    : Label( parent, rect )
  {
    _divinity = divinity;
    _smallTempleCount = smallTempleCount;
    _bigTempleCount = bigTempleCount;
    _mood = 0;

    setFont( Font::create( FONT_1_WHITE ) );
  }

  virtual void _updateTexture( GfxEngine& painter )
  {
    Label::_updateTexture( painter );

    PictureRef& texture = getTextPicture();
    Font font = getFont();

    if( _divinity.isValid() )
    {
      _lastFestival = _divinity->getLastFestivalDate().getMonthToDate( GameDate::current() );
      _mood = _divinity->getRelation();

      font.draw( *texture, _divinity->getName(), 0, 0 );
      Font fontBlack = Font::create( FONT_1 );
      fontBlack.draw( *texture, StringHelper::format( 0xff, "(%s)", _divinity->getShortDescription().c_str() ), 80, 0 );
      font.draw( *texture, StringHelper::format( 0xff, "%d", _smallTempleCount ), 220, 0 );
      font.draw( *texture, StringHelper::format( 0xff, "%d", _bigTempleCount ), 280, 0 );
      font.draw( *texture, StringHelper::format( 0xff, "%d", _lastFestival ), 350, 0 );
      font.draw( *texture, StringHelper::format( 0xff, "%d", _mood ), 420, 0 );
    }
    else
    {
      font.draw( *texture, _("##Oracles_in_city##"), 0, 0 );
      font.draw( *texture, StringHelper::format( 0xff, "%d", _smallTempleCount ), 220, 0 );
    }
  }

private:
  RomeDivinityPtr _divinity;
  int _smallTempleCount;
  int _bigTempleCount;
  int _lastFestival;
  int _mood;
};

class AdvisorReligionWindow::Impl
{
public:
  PictureRef background;

  ReligionInfoLabel* lbCeresInfo;
  ReligionInfoLabel* lbNeptuneInfo;
  ReligionInfoLabel* lbMercuryInfo;
  ReligionInfoLabel* lbMarsInfo;
  ReligionInfoLabel* lbVenusInfo;
  ReligionInfoLabel* lbOracleInfo;
  TexturedButton* btnHelp;

  struct InfrastructureInfo
  {
    int smallTemplCount;
    int bigTempleCount;
  };

  InfrastructureInfo getInfo( CityPtr city, const TileOverlay::Type small, const TileOverlay::Type big )
  {
    CityHelper helper( city );

    InfrastructureInfo ret;

    ret.smallTemplCount = helper.find<ServiceBuilding>( small ).size();
    ret.bigTempleCount = helper.find<ServiceBuilding>( big ).size();

    return ret;
  }
};


AdvisorReligionWindow::AdvisorReligionWindow( CityPtr city, Widget* parent, int id ) 
: Widget( parent, id, Rect( 0, 0, 1, 1 ) ), _d( new Impl )
{
  setGeometry( Rect( Point( (parent->getWidth() - 640 )/2, parent->getHeight() / 2 - 242 ),
               Size( 640, 280 ) ) );

  Label* title = new Label( this, Rect( 60, 10, 60 + 210, 10 + 40) );
  title->setText( _("##Religion advisor##") );
  title->setFont( Font::create( FONT_3 ) );
  title->setTextAlignment( alignUpperLeft, alignCenter );

  _d->background.reset( Picture::create( getSize() ) );

  //main _d->_d->background
  PictureDecorator::draw( *_d->background, Rect( Point( 0, 0 ), getSize() ), PictureDecorator::whiteFrame );
  //buttons _d->_d->background
  PictureDecorator::draw( *_d->background, Rect( 35, 62, getWidth() - 35, 62 + 130 ), PictureDecorator::blackFrame );

  Picture& icon = Picture::load( ResourceGroup::panelBackground, 264 );
  _d->background->draw( icon, Point( 11, 11 ) );

  Font font = Font::create( FONT_1 );
  font.draw( *_d->background, _("##Temples##"), 268, 32, false );
  font.draw( *_d->background, _("##small##"), 240, 47, false );
  font.draw( *_d->background, _("##large##"), 297, 47, false );
  font.draw( *_d->background, _("##Fest.##"), 370, 47, false );
  font.draw( *_d->background, _("##Mood##"), 450, 47, false );

  Point startPoint( 42, 65 );
  Size labelSize( 550, 20 );
  Impl::InfrastructureInfo info = _d->getInfo( city, building::templeCeres, building::B_BIG_TEMPLE_CERES );
  _d->lbCeresInfo = new ReligionInfoLabel( this, Rect( startPoint, labelSize ), DivinePantheon::ceres(), 
                                           info.smallTemplCount, info.bigTempleCount );

  info = _d->getInfo( city, building::templeNeptune, building::B_BIG_TEMPLE_NEPTUNE );
  _d->lbNeptuneInfo = new ReligionInfoLabel( this, Rect( startPoint + Point( 0, 20), labelSize), DivinePantheon::neptune(),
                                             info.smallTemplCount, info.bigTempleCount );

  info = _d->getInfo( city, building::templeMercury, building::B_BIG_TEMPLE_MERCURE );
  _d->lbMercuryInfo = new ReligionInfoLabel( this, Rect( startPoint + Point( 0, 40), labelSize), DivinePantheon::mercury(),
                                             info.smallTemplCount, info.bigTempleCount );

  info = _d->getInfo( city, building::templeMars, building::B_BIG_TEMPLE_MARS );
  _d->lbMarsInfo = new ReligionInfoLabel( this, Rect( startPoint + Point( 0, 60), labelSize), DivinePantheon::mars(),
                                          info.smallTemplCount, info.bigTempleCount );

  info = _d->getInfo( city, building::templeVenus, building::B_BIG_TEMPLE_VENUS );
  _d->lbVenusInfo = new ReligionInfoLabel( this, Rect( startPoint + Point( 0, 80), labelSize), DivinePantheon::venus(),
                                           info.smallTemplCount, info.bigTempleCount );

  info = _d->getInfo( city, building::oracle, building::oracle );
  _d->lbOracleInfo = new ReligionInfoLabel( this, Rect( startPoint + Point( 0, 100), labelSize), RomeDivinityPtr(),
                                            info.smallTemplCount, 0 );

  _d->btnHelp = new TexturedButton( this, Point( 12, getHeight() - 39), Size( 24 ), -1, ResourceMenu::helpInfBtnPicId );
}

void AdvisorReligionWindow::draw( GfxEngine& painter )
{
  if( !isVisible() )
    return;

  painter.drawPicture( *_d->background, getScreenLeft(), getScreenTop() );

  Widget::draw( painter );
}

}//end namespace gui
