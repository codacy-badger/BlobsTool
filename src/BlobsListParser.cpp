/* BlobsListParser.cpp
 *
 * Copyright (C) 2018 MkfsSion <mkfssion@mkfssion.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include <stdexcept>
#include <BlobsListParser.h>

static Blob blob_parse(std::string &line)
{
    if (line.empty())
    {
        return {false};
    }
    Blob b {false};
    if (line[0] == '-')
    {
        b.modularized = true;
    }
    if (line.find(':') != std::string::npos || line.find('|') != std::string::npos)
    {
        size_t pos = 0;
        size_t dpos = 0;
        size_t ppos = 0;
        if ((pos = line.find(':')) != std::string::npos && line.find('|') == std::string::npos)
        {
            b.source = line.substr(b.modularized ? 1 : 0, pos - (b.modularized ? 1 : 0));
            b.destination = line.substr(pos+1);
        }
        if ((pos = line.find('|')) != std::string::npos && line.find(':') == std::string::npos)
        {
            b.source = line.substr(b.modularized ? 1 : 0, pos - (b.modularized ? 1 : 0));
            b.destination = b.source;
            b.sha1sum = line.substr(pos+1);
        }
        if ((dpos = line.find(':')) != std::string::npos && (ppos = line.find('|')) != std::string::npos)
        {
            b.source = line.substr(b.modularized ? 1 : 0, dpos - (b.modularized ? 1 : 0));
            b.destination = line.substr(dpos+1, ppos-dpos-1);
            b.sha1sum = line.substr(ppos+1);
        }
    }
    else
    {
        b.source =line.substr(b.modularized ? 1 : 0);
        b.destination = b.source;
    }
    return b;
}

static BlobsSection blob_section_parse(std::string &line)
{
    if (line.empty())
    {
        return {};
    }
    BlobsSection bs;
    size_t npos = 0;
    size_t vpos = 0;
    if ((npos = line.find('#')) != std::string::npos)
    {
        if ((vpos = line.find("- from") + 6) !=  std::string::npos)
        {
            bs.setSectionName(line.substr(npos+2, vpos-9-npos));
            bs.setVendorName(line.substr(vpos+1));
        }
        else
        {
            bs.setSectionName(line.substr(npos+2));
        }
    }
    return bs;
}

BlobsList BlobsListParser::parse()
{
    blist = new BlobsList;
    std::string line;
    // Start parse line
    while(getline(ifs, line))
    {
        if (line.empty())
            continue;
        if (line[0] == '#')
        {
            BlobsSection bs = blob_section_parse(line);
            blist->addBlobsSection(bs);
            continue;
        }
        if (blist->getSize() == 0)
        {
            BlobsSection bs;
            bs.setSectionName("Unnamed");
            bs.setVendorName("Unknown");
            blist->addBlobsSection(bs);
        }
        auto bs = blist->getBlobsSection(blist->getSize() - 1);
        Blob b = blob_parse(line);
        bs->addBlob(b);
    }
    BlobsList list = *blist;
    delete blist;
    return list;
}