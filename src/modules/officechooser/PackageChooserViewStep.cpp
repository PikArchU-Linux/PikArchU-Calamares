/* === This file is part of Calamares - <https://github.com/calamares> ===
 *
 *   Copyright 2019, Adriaan de Groot <groot@kde.org>
 *   Copyright 2019, Philip Müller <philm@manjaro.org>
 *
 *   Calamares is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Calamares is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with Calamares. If not, see <http://www.gnu.org/licenses/>.
 */

#include "PackageChooserViewStep.h"

#include "PackageChooserPage.h"
#include "PackageModel.h"

#include "GlobalStorage.h"
#include "JobQueue.h"

#include "utils/CalamaresUtilsSystem.h"
#include "utils/Logger.h"
#include "utils/Variant.h"

#include <QDesktopServices>
#include <QVariantMap>

CALAMARES_PLUGIN_FACTORY_DEFINITION( PackageChooserViewStepFactory, registerPlugin< PackageChooserViewStep >(); )

PackageChooserViewStep::PackageChooserViewStep( QObject* parent )
    : Calamares::ViewStep( parent )
    , m_widget( nullptr )
    , m_model( nullptr )
    , m_mode( PackageChooserMode::Exclusive )
{
    emit nextStatusChanged( false );
}


PackageChooserViewStep::~PackageChooserViewStep()
{
    if ( m_widget && m_widget->parent() == nullptr )
    {
        m_widget->deleteLater();
    }
    delete m_model;
}


QString
PackageChooserViewStep::prettyName() const
{
    return tr( "Office Suite" );
}


QWidget*
PackageChooserViewStep::widget()
{
    if ( !m_widget )
    {
        m_widget = new PackageChooserPage( m_mode, nullptr );
        connect( m_widget, &PackageChooserPage::selectionChanged, [=]() {
            emit nextStatusChanged( this->isNextEnabled() );
        } );

        if ( m_model )
        {
            hookupModel();
        }
        else
        {
            cWarning() << "PackageChooser Widget created before model.";
        }
    }
    return m_widget;
}


bool
PackageChooserViewStep::isNextEnabled() const
{
    if ( !m_model )
    {
        return false;
    }

    if ( !m_widget )
    {
        // No way to have changed anything
        return true;
    }

    switch ( m_mode )
    {
    case PackageChooserMode::Optional:
    case PackageChooserMode::Multiple:
        // zero or one OR zero or more
        return true;
    case PackageChooserMode::Exclusive:
    case PackageChooserMode::RequiredMultiple:
        // exactly one OR one or more
        return m_widget->hasSelection();
    }

    NOTREACHED return true;
}


bool
PackageChooserViewStep::isBackEnabled() const
{
    return true;
}


bool
PackageChooserViewStep::isAtBeginning() const
{
    return true;
}


bool
PackageChooserViewStep::isAtEnd() const
{
    return true;
}


void
PackageChooserViewStep::onLeave()
{
}

Calamares::JobList
PackageChooserViewStep::jobs() const
{
    Calamares::JobList l;
    return l;
}

void
PackageChooserViewStep::setConfigurationMap( const QVariantMap& configurationMap )
{
    // TODO: use the configurationMap

    if ( !m_model )
    {

        m_model = new PackageListModel( nullptr );
        m_model->addPackage( PackageItem { "libreoffice-still", "libreoffice-still", "LibreOffice", "LibreOffice is a powerful and free office suite, used by millions of people around the world. ts clean interface and feature-rich tools help you unleash your creativity and enhance your productivity.", ":/images/LibreOffice.jpg" } );
        m_model->addPackage(
            PackageItem { "freeoffice", "freeoffice", "FreeOffice", "FreeOffice 2018 is a full-featured Office suite with word processing, spreadsheet and presentation software. It is seamlessly compatible with Microsoft Office. (Note: You need to register the product for free longterm usage)", ":/images/FreeOffice.jpg" } );


        if ( m_widget )
        {
            hookupModel();
        }
    }
}

void
PackageChooserViewStep::hookupModel()
{
    if ( !m_model || !m_widget )
    {
        cError() << "Can't hook up model until widget and model both exist.";
        return;
    }

    m_widget->setModel( m_model );
}
