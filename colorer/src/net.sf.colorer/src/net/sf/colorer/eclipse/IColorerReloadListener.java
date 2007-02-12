package net.sf.colorer.eclipse;

/**
 * HRC database reload listener
 */
public interface IColorerReloadListener {
    
    /** Called when HRC database is reloaded */
    void notifyReload();
}
