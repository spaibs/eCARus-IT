package com.example.gtflauncher.navigation;

public class NavigationJNIAccess
{
    private NavigationJNIAccess()
    {
    }

    // @TUM: Die folgenden methoden werden per JNI aus der C++ GUIDE-Applikation aufgerufen
    public static void onTouchMove(int deltaX, int deltaY)
    {
        final NavigationService service = NavigationService.getInstance();
        service.scrollMapCenter(deltaX, deltaY);
    }

    public static void onTouchRotate(float rotationAngle)
    {
        final NavigationService service = NavigationService.getInstance();
        service.rotateMap(rotationAngle);
    }

    public static void onTouchZoom(float zoomFactor)
    {
        final NavigationService service = NavigationService.getInstance();
        service.zoomMap(zoomFactor);
    }

    public static void onCityIndex(int cityIndex)
    {
        final NavigationService service = NavigationService.getInstance();
        service.selectCityMap(cityIndex);
    }

    public static void onDoubleTap(int tapX, int tapY)
    {
        // TODO: Definieren, was in der Karte passieren soll, dann implementieren!
    }
}
