/*---------------------------------------------------------------------\
|								       |
|		       __   __	  ____ _____ ____		       |
|		       \ \ / /_ _/ ___|_   _|___ \		       |
|			\ V / _` \___ \ | |   __) |		       |
|			 | | (_| |___) || |  / __/		       |
|			 |_|\__,_|____/ |_| |_____|		       |
|								       |
|				core system			       |
|							 (C) SuSE GmbH |
\----------------------------------------------------------------------/

  File:	      YQPkgRpmGroupTagsFilterView.cc

  Author:     Stefan Hundhammer <sh@suse.de>

  Textdomain "qt-pkg"

/-*/


#define YUILogComponent "qt-pkg"
#include "YUILog.h"

#include <QApplication>

#include "YQPackageSelector.h"
#include "zypp/sat/LookupAttr.h"

#include "YQPkgRpmGroupTagsFilterView.h"
#include "YQi18n.h"
#include "utf8.h"

/**
 * translations taken from packagekit
 **/
QString
group_enum_to_localised_text (YPkgGroupEnum group)
{
  QString text;
	switch (group)
  {
	case PK_GROUP_ENUM_ACCESSIBILITY:
		text = _("Accessibility");
		break;
	case PK_GROUP_ENUM_ACCESSORIES:
		text = _("Accessories");
		break;
	case PK_GROUP_ENUM_EDUCATION:
		text = _("Education");
		break;
	case PK_GROUP_ENUM_GAMES:
		text = _("Games");
		break;
	case PK_GROUP_ENUM_GRAPHICS:
		text = _("Graphics");
		break;
	case PK_GROUP_ENUM_INTERNET:
		text = _("Internet");
		break;
	case PK_GROUP_ENUM_OFFICE:
		text = _("Office");
		break;
	case PK_GROUP_ENUM_OTHER:
		text = _("Other");
		break;
	case PK_GROUP_ENUM_PROGRAMMING:
		text = _("Programming");
		break;
	case PK_GROUP_ENUM_MULTIMEDIA:
		text = _("Multimedia");
		break;
	case PK_GROUP_ENUM_SYSTEM:
		text = _("System");
		break;
	case PK_GROUP_ENUM_DESKTOP_GNOME:
		text = _("GNOME desktop");
		break;
	case PK_GROUP_ENUM_DESKTOP_KDE:
		text = _("KDE desktop");
		break;
	case PK_GROUP_ENUM_DESKTOP_XFCE:
		text = _("XFCE desktop");
		break;
	case PK_GROUP_ENUM_DESKTOP_OTHER:
		text = _("Other desktops");
		break;
	case PK_GROUP_ENUM_PUBLISHING:
		text = _("Publishing");
		break;
	case PK_GROUP_ENUM_SERVERS:
		text = _("Servers");
		break;
	case PK_GROUP_ENUM_FONTS:
		text = _("Fonts");
		break;
	case PK_GROUP_ENUM_ADMIN_TOOLS:
		text = _("Admin tools");
		break;
	case PK_GROUP_ENUM_LEGACY:
		text = _("Legacy");
		break;
	case PK_GROUP_ENUM_LOCALIZATION:
		text = _("Localization");
		break;
	case PK_GROUP_ENUM_VIRTUALIZATION:
		text = _("Virtualization");
		break;
	case PK_GROUP_ENUM_SECURITY:
		text = _("Security");
		break;
	case PK_GROUP_ENUM_POWER_MANAGEMENT:
		text = _("Power management");
		break;
	case PK_GROUP_ENUM_COMMUNICATION:
		text = _("Communication");
		break;
	case PK_GROUP_ENUM_NETWORK:
		text = _("Network");
		break;
	case PK_GROUP_ENUM_MAPS:
		text = _("Maps");
		break;
	case PK_GROUP_ENUM_REPOS:
		text = _("Software sources");
		break;
      case YPKG_GROUP_ALL:
          text = _("All packages");
          break;
          
      case YPKG_GROUP_SUGGESTED:
          text = _("Suggested packages");
          break;
          
      case YPKG_GROUP_RECOMMENDED:
          text = _("Recommended packages");
          break;
          
	case PK_GROUP_ENUM_UNKNOWN:
		text = _("Unknown group");
		break;
	default:
      break;
      
	}
	return text;
}

std::string
group_enum_to_icon (YPkgGroupEnum group)
{
  std::string text;
	switch (group)
  {
	case PK_GROUP_ENUM_ACCESSIBILITY:
		text = "package_main";
		break;
	case PK_GROUP_ENUM_ACCESSORIES:
		text = "package_applications";
		break;
	case PK_GROUP_ENUM_EDUCATION:
		text = "package_edutainment";
		break;
	case PK_GROUP_ENUM_GAMES:
		text = "package_games";
		break;
	case PK_GROUP_ENUM_GRAPHICS:
		text = "package_graphics";
		break;
	case PK_GROUP_ENUM_INTERNET:
		text = "package_network";
		break;
	case PK_GROUP_ENUM_OFFICE:
		text = "package_office_addressbook";
		break;
	case PK_GROUP_ENUM_OTHER:
		text = "package_main";
		break;
	case PK_GROUP_ENUM_PROGRAMMING:
		text = "package_development";
		break;
	case PK_GROUP_ENUM_MULTIMEDIA:
		text = "package_multimedia";
		break;
	case PK_GROUP_ENUM_SYSTEM:
		text = "yast-system";
		break;
	case PK_GROUP_ENUM_DESKTOP_GNOME:
		text = "pattern-gnome";
		break;
	case PK_GROUP_ENUM_DESKTOP_KDE:
		text = "pattern-kde";
		break;
	case PK_GROUP_ENUM_DESKTOP_XFCE:
      text = "package_utility_desktop";
		break;
	case PK_GROUP_ENUM_DESKTOP_OTHER:
		text = "package_utility_desktop";
		break;
	case PK_GROUP_ENUM_PUBLISHING:
		text = "package_main";
		break;
	case PK_GROUP_ENUM_SERVERS:
		text = "package_editors";
		break;
	case PK_GROUP_ENUM_FONTS:
		text = "package_main";
		break;
	case PK_GROUP_ENUM_ADMIN_TOOLS:
		text = "yast-sysconfig";
		break;
	case PK_GROUP_ENUM_LEGACY:
      text = "package_main";
		break;
	case PK_GROUP_ENUM_LOCALIZATION:
		text = "yast-language";
		break;
	case PK_GROUP_ENUM_VIRTUALIZATION:
		text = "yast-create-new-vm";
		break;
	case PK_GROUP_ENUM_SECURITY:
		text = "yast-security";
		break;
	case PK_GROUP_ENUM_POWER_MANAGEMENT:
		text = "package_settings_power";
		break;
	case PK_GROUP_ENUM_COMMUNICATION:
		text = "yast-modem";
		break;
	case PK_GROUP_ENUM_NETWORK:
		text = "package_network";
		break;
	case PK_GROUP_ENUM_MAPS:
		text = "package_main";
		break;
	case PK_GROUP_ENUM_REPOS:
		text = "package_main";
		break;
  case YPKG_GROUP_SUGGESTED:
      case YPKG_GROUP_RECOMMENDED:
          text = "package_edutainment_languages";
          
    break;         
	case PK_GROUP_ENUM_UNKNOWN:
  case YPKG_GROUP_ALL:
		text = "package_main";
		break;
	default:
      break;
      
	}
	return text;
}


YPkgGroupEnum
zypp_get_group (const std::string &groupu)
{
    // TODO Look for a faster and nice way to do this conversion

    std::string group = zypp::str::toLower(groupu);    

    if (group.find ("amusements") != std::string::npos) {
        return PK_GROUP_ENUM_GAMES;
    } else if (group.find ("development") != std::string::npos) {
        return PK_GROUP_ENUM_PROGRAMMING;
    } else if (group.find ("hardware") != std::string::npos) {
        return PK_GROUP_ENUM_SYSTEM;
    } else if (group.find ("archiving") != std::string::npos 
               || group.find("clustering") != std::string::npos
               || group.find("system/monitoring") != std::string::npos
               || group.find("databases") != std::string::npos
               || group.find("system/management") != std::string::npos) {
        return PK_GROUP_ENUM_ADMIN_TOOLS;
    } else if (group.find ("graphics") != std::string::npos) {
        return PK_GROUP_ENUM_GRAPHICS;
    } else if (group.find ("multimedia") != std::string::npos) {
        return PK_GROUP_ENUM_MULTIMEDIA;
    } else if (group.find ("network") != std::string::npos) {
        return PK_GROUP_ENUM_NETWORK;
    } else if (group.find ("office") != std::string::npos 
               || group.find("text") != std::string::npos
               || group.find("editors") != std::string::npos) {
        return PK_GROUP_ENUM_OFFICE;
    } else if (group.find ("publishing") != std::string::npos) {
        return PK_GROUP_ENUM_PUBLISHING;
    } else if (group.find ("security") != std::string::npos) {
        return PK_GROUP_ENUM_SECURITY;
    } else if (group.find ("telephony") != std::string::npos) {
        return PK_GROUP_ENUM_COMMUNICATION;
    } else if (group.find ("gnome") != std::string::npos) {
        return PK_GROUP_ENUM_DESKTOP_GNOME;
    } else if (group.find ("kde") != std::string::npos) {
        return PK_GROUP_ENUM_DESKTOP_KDE;
    } else if (group.find ("xfce") != std::string::npos) {
        return PK_GROUP_ENUM_DESKTOP_XFCE;
    } else if (group.find ("gui/other") != std::string::npos) {
        return PK_GROUP_ENUM_DESKTOP_OTHER;
    } else if (group.find ("localization") != std::string::npos) {
        return PK_GROUP_ENUM_LOCALIZATION;
    } else if (group.find ("system") != std::string::npos) {
        return PK_GROUP_ENUM_SYSTEM;
    } else if (group.find ("scientific") != std::string::npos) {
        return PK_GROUP_ENUM_EDUCATION;
    }
    
    return PK_GROUP_ENUM_UNKNOWN;
}

YQPkgRpmGroupTagsFilterView::YQPkgRpmGroupTagsFilterView( QWidget * parent )
    : QTreeWidget( parent )
    , _selectedGroup(YPKG_GROUP_ALL)
{   
    setIconSize(QSize(32,32));
    setHeaderLabels( QStringList(_( "Package Groups" )) );
    setRootIsDecorated( false );
    setSortingEnabled( true );
    connect( this, SIGNAL( currentItemChanged     ( QTreeWidgetItem *, QTreeWidgetItem * ) ),
	     this, SLOT  ( slotSelectionChanged	( QTreeWidgetItem * ) ) );
    fillGroups();
    
    selectSomething();
}


YQPkgRpmGroupTagsFilterView::~YQPkgRpmGroupTagsFilterView()
{
}

void YQPkgRpmGroupTagsFilterView::fillGroups()
{
    if ( _groupsMap.empty() )
    {
        zypp::sat::LookupAttr rpmgroups( zypp::sat::SolvAttr::group );
        for ( zypp::sat::LookupAttr::iterator it = rpmgroups.begin();
              it != rpmgroups.end();
              ++it )
        {
            YPkgGroupEnum group;
            std::map<std::string, YPkgGroupEnum>::const_iterator itg = _groupsCache.find(it.asString());
            if ( itg == _groupsCache.end() )
            {
                group = zypp_get_group( it.asString() );
                _groupsCache[it.asString()] = group;
            } 
            else 
            {
                group = itg->second;
            }
            
            //std::cout << it.asString() << std::endl;
            
            if ( _groupsMap.find(group) == _groupsMap.end() )
            {
                _groupsMap[group] =  new YQPkgRpmGroupTag( this, group );
            }
        }
	
	_groupsMap[YPKG_GROUP_ALL        ] =  new YQPkgRpmGroupTag( this, YPKG_GROUP_ALL );
	_groupsMap[YPKG_GROUP_RECOMMENDED] =  new YQPkgRpmGroupTag( this, YPKG_GROUP_RECOMMENDED );
	_groupsMap[YPKG_GROUP_SUGGESTED  ] =  new YQPkgRpmGroupTag( this, YPKG_GROUP_SUGGESTED );

    }
}

void
YQPkgRpmGroupTagsFilterView::selectSomething()
{
// FIXME
//     QTreeWidgetItem * item = children().first();
// 
//     if ( item )
// 	setCurrentItem(item);
}


void
YQPkgRpmGroupTagsFilterView::filterIfVisible()
{
    if ( isVisible() )
	filter();
}


void
YQPkgRpmGroupTagsFilterView::filter()
{
    emit filterStart();
    // yuiDebug() << "Filtering packages for RPM group \"" << selectedRpmGroup() << "\"" << endl;
    
    if ( selection() )
    {
        for ( ZyppPoolIterator it = zyppPkgBegin();
              it != zyppPkgEnd();
              ++it )
        {
            ZyppSel selectable = *it;

            // Multiple instances of this package may or may not be in the same
            // RPM group, so let's check both the installed version (if there
            // is any) and the candidate version.
            //
            // Make sure we emit only one filterMatch() signal if both exist
            // and both are in the same RPM group. We don't want multiple list
            // entries for the same package!

            bool match =
                check( selectable, tryCastToZyppPkg( selectable->candidateObj() ) ) ||
                check( selectable, tryCastToZyppPkg( selectable->installedObj() ) );

            // If there is neither an installed nor a candidate package, check
            // any other instance.

            if ( ! match			&&
                 ! selectable->candidateObj()   &&
                 ! selectable->installedObj()	  )
                check( selectable, tryCastToZyppPkg( selectable->theObj() ) );
        }
    }

    emit filterFinished();
}


void
YQPkgRpmGroupTagsFilterView::slotSelectionChanged( QTreeWidgetItem * newSelection )
{
    YQPkgRpmGroupTag * sel = dynamic_cast<YQPkgRpmGroupTag *>( newSelection );

    if ( sel )
    {
        _selectedGroup = sel->group();
        // for the list of reccommended packages, we need
        // to solve first
        if ( _selectedGroup == YPKG_GROUP_SUGGESTED ||
             _selectedGroup == YPKG_GROUP_RECOMMENDED )
        {
            // set the busy cursor for the solving
            QApplication::setOverrideCursor(Qt::WaitCursor);
            zypp::getZYpp()->resolver()->resolvePool();
            QApplication::restoreOverrideCursor();
        }
        
    }
    else
    {
        _selectedGroup = YPKG_GROUP_ALL;
    }

    filter();
}


bool
YQPkgRpmGroupTagsFilterView::check( ZyppSel	selectable,
				    ZyppPkg	pkg		)
{
    if ( ! pkg || ! selection() )
	return false;

    if ( selection()->group() == YPKG_GROUP_ALL )		// Special case: All packages
    {
        emit filterMatch( selectable, pkg );
        return true;
    }

    YPkgGroupEnum groupn = zypp_get_group(pkg->group());
    
    if ( groupn == selectedGroup() )
    {
        emit filterMatch( selectable, pkg );
        return true;
    }

    if ( selectable->candidateObj() )
    {   
        if ( selectedGroup() == YPKG_GROUP_RECOMMENDED &&
             zypp::PoolItem(pkg).status().isRecommended() )
        {
            emit filterMatch( selectable, pkg );
            return true;
        }
        if ( selectedGroup() == YPKG_GROUP_SUGGESTED &&
             zypp::PoolItem(pkg).status().isSuggested() )
        {
            emit filterMatch( selectable, pkg );
            return true;
        }
    }

    return false;
}


YQPkgRpmGroupTag *
YQPkgRpmGroupTagsFilterView::selection() const
{
    QTreeWidgetItem * item = currentItem();

    if ( ! item )
        return 0;

    return dynamic_cast<YQPkgRpmGroupTag *> ( item );
}


YQPkgRpmGroupTag::YQPkgRpmGroupTag( YQPkgRpmGroupTagsFilterView * 	parentFilterView,
                                    YPkgGroupEnum group )
    : QTreeWidgetItem( parentFilterView )
    , _filterView( parentFilterView )
    , _group( group )
{
    QFont f = font(0);
    QFontMetrics fm(f);
    f.setPixelSize( fm.height() * 1.1 );
    setFont(0,f);

    std::string iconName = group_enum_to_icon(group);
    setText( 0, group_enum_to_localised_text(group));
    setIcon(0, QIcon(QString(YQPackageSelector::iconPath(iconName, 32).c_str())));
}

YQPkgRpmGroupTag::~YQPkgRpmGroupTag()
{
    // NOP
}

bool YQPkgRpmGroupTag::operator< ( const QTreeWidgetItem & otherListViewItem ) const
{
    const YQPkgRpmGroupTag * otherCategoryItem = dynamic_cast<const YQPkgRpmGroupTag *>(&otherListViewItem);

    return group() > otherCategoryItem->group();
}


#include "YQPkgRpmGroupTagsFilterView.moc"
