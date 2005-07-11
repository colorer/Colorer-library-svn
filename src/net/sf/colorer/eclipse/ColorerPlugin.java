package net.sf.colorer.eclipse;

import java.net.URL;
import java.util.MissingResourceException;
import java.util.ResourceBundle;
import java.util.Vector;

import net.sf.colorer.ParserFactory;
import net.sf.colorer.impl.Logger;
import net.sf.colorer.swt.ColorManager;

import org.eclipse.core.resources.IWorkspace;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.Platform;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.preference.IPreferenceStore;
import org.eclipse.ui.plugin.AbstractUIPlugin;
import org.osgi.framework.BundleContext;


/**
 * The main plugin class to be used in the desktop.
 */
public class ColorerPlugin extends AbstractUIPlugin {
    //The shared instance.
    private static ColorerPlugin plugin;
    private ResourceBundle resourceBundle;
    
    private String catalogPath;
    private ParserFactory parserFactory;
    private ColorManager colorManager = new ColorManager();
    private Vector reloadListeners = new Vector();

    /**
     * The constructor.
     */
    public ColorerPlugin() {
        super();
        plugin = this;
    }

    /**
     * This method is called upon plug-in activation
     */
    public void start(BundleContext context) throws Exception {
        super.start(context);
        
        plugin = this;
        Logger.trace("Plugin", "Loaded");
    
        IPreferenceStore store = getPreferenceStore();
        store.setDefault(PreferencePage.SPACES_FOR_TABS, false);
        store.setDefault(PreferencePage.WORD_WRAP, false);
        store.setDefault(PreferencePage.TAB_WIDTH, 4);
    
        store.setDefault(PreferencePage.FULL_BACK, false);
        store.setDefault(PreferencePage.USE_BACK, false);
        store.setDefault(PreferencePage.VERT_CROSS, false);
        store.setDefault(PreferencePage.HORZ_CROSS, true);
        store.setDefault(PreferencePage.PAIRS_MATCH, "PAIRS_OUTLINE");
    
        store.setDefault(PreferencePage.HRD_SET, "default");
    
        store.setDefault("Outline.Hierarchy", true);
        store.setDefault("Outline.Sort", false);

        store.setDefault("RegionsTree.Link", false);

        store.setDefault("g.Prefix", "");
        store.setDefault("g.Suffix", ".html");
        store.setDefault("g.HRDSchema", store.getString(PreferencePage.HRD_SET));
        store.setDefault("g.HtmlHeaderFooter", true);
        store.setDefault("g.InfoHeader", true);
        store.setDefault("g.UseLineNumbers", true);
        store.setDefault("g.OutputEncoding", "default");
        store.setDefault("g.TargetDirectory", "/");
        store.setDefault("g.LinkSource", "");
    }

    /**
     * This method is called when the plug-in is stopped
     */
    public void stop(BundleContext context) throws Exception {
        super.stop(context);
        plugin = null;
        resourceBundle = null;
    }

    /**
     * Returns the shared instance.
     */
    public static ColorerPlugin getDefault() {
        return plugin;
    }

    public synchronized ParserFactory getParserFactory() {
        if (parserFactory != null) {
            return parserFactory;
        }
        try {
            catalogPath = new URL(Platform.resolve(
                    getBundle().getEntry("/")), "colorer/catalog.xml").toExternalForm();
            Logger.trace("EclipsecolorerPlugin", "Catalog: "+catalogPath);
            parserFactory = new ParserFactory(catalogPath);
        } catch (Throwable e) {
            Logger.trace("EclipsecolorerPlugin", "Fault in getting parser factory", e);
            boolean error = true;
            Throwable exc = e;
            try {
                parserFactory = new ParserFactory();
                error = false;
            } catch (Throwable e1) {
                error = true;
                exc = e1;
            }
            if (error) {
                MessageDialog.openError(null, Messages.get("init.error.title"),
                                Messages.get("init.error.pf") + "\n"
                                        + exc.getMessage());
            }
        }
        return parserFactory;
    };

    public synchronized void reloadParserFactory() {
        // informs all the editors about ParserFactory reloading
        if (parserFactory != null && !parserFactory.isDisposed()) {
            parserFactory.dispose();
        }
        parserFactory = null;
        parserFactory = getParserFactory();
        notifyReloadListeners();
    };

    void notifyReloadListeners() {
        for(int idx = 0; idx < reloadListeners.size(); idx++) {
            ((IColorerReloadListener)reloadListeners.elementAt(idx)).notifyReload();
        }
            
    }
    
    /**
     * Adds HRC database reload action listener
     * */
    public void addReloadListener(IColorerReloadListener listener) {
        if (!reloadListeners.contains(listener)) {
            reloadListeners.add(listener);
        }
    }
    
    /**
     * Adds HRC database reload action listener
     * */
    public void removeReloadListener(IColorerReloadListener listener) {
        reloadListeners.remove(listener);
    }

    
    public ColorManager getColorManager() {
        return colorManager;
    }

    /**
     * Returns the workspace instance.
     */
    public static IWorkspace getWorkspace() {
        return ResourcesPlugin.getWorkspace();
    }
    
    /**
     * Returns the plugin's resource bundle,
     */
    public ResourceBundle getResourceBundle() {
        try {
            if (resourceBundle == null)
                resourceBundle = ResourceBundle.getBundle("net.sf.colorer.eclipse.EclipsecolorerPluginResources");
        } catch (MissingResourceException x) {
            resourceBundle = null;
        }
        return resourceBundle;
    }    
}
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
 * Cail Lomecb <cail@nm.ru>.
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