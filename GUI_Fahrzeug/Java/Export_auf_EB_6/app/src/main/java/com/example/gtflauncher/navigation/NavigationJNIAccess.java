package com.example.gtflauncher.navigation;

public class NavigationJNIAccess
{
    private NavigationService service;
    private static NavigationJNIAccess instance = null;

    protected NavigationJNIAccess()
    {
        // Exists only to defeat instantiation.
    }

    public static NavigationJNIAccess getInstance()
    {
        if(null == instance)
        {
            instance = new NavigationJNIAccess();
        }
        return instance;
    }

    public void registerNavigationService(final NavigationService service)
    {
        this.service = service;
    }

    public NavigationService getNavigationService()
    {
        return service;
    }

    // @TUM: Die folgenden methoden werden per JNI aus der C++ GUIDE-Applikation aufgerufen
    public static void onTouchMove(int deltaX, int deltaY)
    {
        NavigationJNIAccess.getInstance().getNavigationService().scrollMapCenter(deltaX, deltaY);
    }

    public static void onTouchRotate(float rotationAngle)
    {
        NavigationJNIAccess.getInstance().getNavigationService().rotateMap(rotationAngle);
    }

    public static void onTouchZoom(float zoomFactor)
    {
        NavigationJNIAccess.getInstance().getNavigationService().zoomMap(zoomFactor);
    }

    public static void onDoubleTap(int tapX, int tapY)
    {
        // TODO: Definieren, was in der Karte passieren soll, dann implementieren!
    }
}
