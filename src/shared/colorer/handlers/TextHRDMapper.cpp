
#include<stdio.h>
#include<xml/xml.h>
#include<colorer/handlers/TextHRDMapper.h>

TextHRDMapper::TextHRDMapper(){};
TextHRDMapper::~TextHRDMapper(){
  int idx;
  for(idx = 0; idx < regionDefines.size(); idx++){
    const TextRegion *rd = TextRegion::cast(regionDefines.get(regionDefines.key(idx)));
    delete rd->stext; delete rd->etext;
    delete rd->sback; delete rd->eback;
    delete rd;
  };
};

/** Loads region definitions from HRD file.
    Multiple files could be loaded.
*/
void TextHRDMapper::loadRegionMappings(InputSource *is)
{
CXmlEl *hrdbase, *hbase;

  hbase = hrdbase = new CXmlEl();
  const byte *data = is->openStream();
  hbase->parse(data, is->length());
  is->closeStream();

  while((hrdbase = hrdbase->next()) != null)
    if (!hrdbase || hrdbase->getType() == EL_BLOCKED && hrdbase->getName() && *hrdbase->getName() == "hrd") break;
  if (!hrdbase){
    delete hbase;
    throw Exception(DString("Error loading HRD file"));
  };

  for(CXmlEl *curel = hrdbase->child(); curel; curel = curel->next()){
    if (curel->getName() && *curel->getName() == "assign"){
      if (!curel->getParamValue(DString("name"))) continue;
      const String *name = curel->getParamValue(DString("name"));

      if (regionDefines.get(name) != null){
        const TextRegion *rd = TextRegion::cast(regionDefines.get(name));
        delete rd->stext; delete rd->etext;
        delete rd->sback; delete rd->eback;
        delete rd;
      };
      const String *stext = null;
      const String *etext = null;
      const String *sback = null;
      const String *eback = null;
      const String *sval;
      sval = curel->getParamValue(DString("stext"));
      if (sval != null) stext = new SString(sval);
      sval = curel->getParamValue(DString("etext"));
      if (sval != null) etext = new SString(sval);
      sval = curel->getParamValue(DString("sback"));
      if (sval != null) sback = new SString(sval);
      sval = curel->getParamValue(DString("eback"));
      if (sval != null) eback = new SString(sval);

      RegionDefine *rdef = new TextRegion(stext, etext, sback, eback);
      regionDefines.put(name, rdef);
    };
  };
  delete hbase;
};

/** Writes all currently loaded region definitions into
    XML file. Note, that this method writes all loaded
    defines from all loaded HRD files.
*/
void TextHRDMapper::saveRegionMappings(Writer *writer) const
{
  writer->write(DString("<?xml version=\"1.0\"?>\n\
<!DOCTYPE hrd SYSTEM \"../hrd.dtd\">\n\n\
<hrd>\n"));
  for(int i = 0; i <= regionDefines.size(); i++){

    const TextRegion *rdef = TextRegion::cast(regionDefines.get(regionDefines.key(i)));
    writer->write(StringBuffer("  <define name='")+regionDefines.key(i)+"'");
    if (rdef->stext != null) writer->write(StringBuffer(" stext='")+rdef->stext+"'");
    if (rdef->etext != null) writer->write(StringBuffer(" etext='")+rdef->etext+"'");
    if (rdef->sback != null) writer->write(StringBuffer(" sback='")+rdef->sback+"'");
    if (rdef->eback != null) writer->write(StringBuffer(" eback='")+rdef->eback+"'");
    writer->write(DString("/>\n"));
  };
  writer->write(DString("\n</hrd>\n"));
};

/** Adds or replaces region definition */
void TextHRDMapper::setRegionDefine(const String &name, const RegionDefine *rd)
{
  const TextRegion *rd_new = TextRegion::cast(rd);
  const String *stext = null;
  const String *etext = null;
  const String *sback = null;
  const String *eback = null;
  if (rd_new->stext != null) stext = new SString(rd_new->stext);
  if (rd_new->etext != null) etext = new SString(rd_new->etext);
  if (rd_new->sback != null) sback = new SString(rd_new->sback);
  if (rd_new->eback != null) eback = new SString(rd_new->eback);

  RegionDefine *rd_old = regionDefines.get(&name);
  if (rd_old != null){
    const TextRegion *rdef = TextRegion::cast(rd_old);
    delete rdef->stext; delete rdef->etext;
    delete rdef->sback; delete rdef->eback;
    delete rdef;
  };

  RegionDefine *new_region = new TextRegion(stext, etext, sback, eback);
  regionDefines.put(&name, new_region);

  // Searches and replaces old region references
  for(int idx = 0; idx < regionDefinesVector.size(); idx++)
  if (regionDefinesVector.elementAt(idx) == rd_old){
    regionDefinesVector.setElementAt(new_region, idx);
    break;
  };
};

/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is the Colorer Library.
 *
 * The Initial Developer of the Original Code is
 * Cail Lomecb <ruiv@uic.nnov.ru>.
 * Portions created by the Initial Developer are Copyright (C) 1999-2003
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */
