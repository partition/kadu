/*
 * Copyright 2010 Michał Obrembski (byku@byku.com.pl)
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "configuration/configuration-file.h"

#include "image-link-configuration.h"

ImageLinkConfiguration::ImageLinkConfiguration()
{
	configurationUpdated();
}

ImageLinkConfiguration::~ImageLinkConfiguration()
{
}

void ImageLinkConfiguration::createDefaultConfiguration()
{
	config_file.addVariable("Imagelink", "show_yt", true);
	config_file.addVariable("Imagelink", "show_image", true);
	config_file.addVariable("Imagelink", "autoplay", true);
}

void ImageLinkConfiguration::configurationUpdated()
{
	ShowVideos = config_file.readBoolEntry("Imagelink", "show_yt", true);
	AutoStartVideos = config_file.readBoolEntry("Imagelink", "autoplay", true);
	ShowImages = config_file.readBoolEntry("Imagelink", "show_image", true);
}
