package com.example.gtflauncher.navigation;

import android.app.Activity;
import android.os.Handler;
import android.util.Log;
import android.view.ViewGroup;
import android.widget.FrameLayout;

import com.example.gtflauncher.R;

import org.mapsforge.core.model.LatLong;
import org.mapsforge.map.android.graphics.AndroidGraphicFactory;
import org.mapsforge.map.android.util.AndroidUtil;
import org.mapsforge.map.android.view.MapView;
import org.mapsforge.map.layer.cache.TileCache;
import org.mapsforge.map.layer.renderer.TileRendererLayer;
import org.mapsforge.map.reader.MapDataStore;
import org.mapsforge.map.reader.MapFile;
import org.mapsforge.map.rendertheme.InternalRenderTheme;

import java.io.File;

public class NavigationService
{
    private final static String LOG_TAG = "NavigationService";

    private static NavigationService instance = null;
    public static NavigationService getInstance()
    {
        if(null == instance)
        {
            instance = new NavigationService();
        }
        return instance;
    }

    private MapView mapView;
    private TileCache tileCache;
    private TileRendererLayer tileRendererLayer;
    private File mapFile;

    private Activity appActivity;

    private boolean navigationStarted = false;

    public void init(final Activity appActivity, final String modelPath)
    {
        this.appActivity = appActivity;

        //See: http://ftp-stud.hs-esslingen.de/pub/Mirrors/download.mapsforge.org/maps/
        mapFile = new File(modelPath + "/maps/", "bayern.map");
        if(!mapFile.isFile())
        {
            Log.e(LOG_TAG, "mapFile '" + mapFile.getAbsoluteFile() + "' couldn't be found.");
        }

        final Handler handler = new Handler();
        handler.postDelayed(new Runnable() {
            @Override
            public void run() {
                prepareNavigation();
            }
        }, 1000);
    }

    public void close()
    {
        mapView.destroyAll();
    }

    public void prepareNavigation()
    {
        prepareMap();
        startNavigation();
    }

    public void prepareMap()
    {
        // Initialize Graphic Factory for MapForge
        // Important: Do this before starting navigation
        AndroidGraphicFactory.createInstance(appActivity.getApplication());

        // Init MapView
        mapView = new MapView(appActivity);
        mapView.setLayoutParams(new ViewGroup.LayoutParams(
                ViewGroup.LayoutParams.MATCH_PARENT,
                ViewGroup.LayoutParams.MATCH_PARENT));

        FrameLayout layout = (FrameLayout) appActivity.findViewById(R.id.rootLayout);
        layout.addView(mapView);

        mapView.setClickable(false);
        mapView.getMapScaleBar().setVisible(true);
        mapView.setBuiltInZoomControls(false);
        mapView.setFocusable(false);
        mapView.getMapZoomControls().setZoomLevelMin((byte) 10);
        mapView.getMapZoomControls().setZoomLevelMax((byte) 20);

        // create a tile cache of suitable size
        tileCache = AndroidUtil.createTileCache(appActivity, "mapcache",
                mapView.getModel().displayModel.getTileSize(), 1f,
                mapView.getModel().frameBufferModel.getOverdrawFactor());

        layout.invalidate();
    }

    public void startNavigation()
    {
        if(!mapFile.isFile())
        {
            return;
        }

        // 48.779209, 9.1772152 = Stuttgart ; 48.1509834021902,  11.56798113464049 = eCARus-Keller
        mapView.getModel().mapViewPosition.setCenter(new LatLong(48.1509834021902, 11.56798113464049));
        mapView.getModel().mapViewPosition.setZoomLevel((byte) 18);

        // tile renderer layer using internal render theme
        MapDataStore mapDataStore = new MapFile(mapFile);
        tileRendererLayer = new TileRendererLayer(tileCache, mapDataStore,
                mapView.getModel().mapViewPosition, false, true, AndroidGraphicFactory.INSTANCE);
        tileRendererLayer.setXmlRenderTheme(InternalRenderTheme.OSMARENDER);

        // only once a layer is associated with a mapView the rendering starts
        mapView.getLayerManager().getLayers().add(tileRendererLayer);

        navigationStarted = true;
    }

    public void scrollMapCenter(int deltaX, int deltaY)
    {
        if(!navigationStarted)
        {
            startNavigation();
        }
        mapView.getModel().mapViewPosition.moveCenter(deltaX, deltaY);
    }
    int counter = 0;
    public void rotateMap(float rotationAngle)
    {
        // TODO: Überprüfen, ob korrekt
        Log.d(LOG_TAG, "rotateMap rotationAngle=" + rotationAngle);
       // mapView.setRotation(mapView.getRotation() + rotationAngle);
    }

    public float zoomLast=0;
    public float zoomSum=0;

    public void zoomMap(float zoomFactor)
    {
        // TODO: Überprüfen, ob korrekt
        Log.d(LOG_TAG, "zoomMap zoomFactor=" + zoomFactor);

        zoomSum = Math.abs(zoomLast - zoomFactor);
        if(zoomSum>0.3) {
            zoomLast = zoomFactor;
                if (zoomFactor < 1.0f) {
                    mapView.getModel().mapViewPosition.zoomOut();

                }
                else if (zoomFactor > 1.0f) {
                    mapView.getModel().mapViewPosition.zoomIn();

                }
        }
        if (mapView.getModel().mapViewPosition.getZoomLevel()<10) {
            mapView.getModel().mapViewPosition.setZoomLevel((byte) 10);
        }
        if (mapView.getModel().mapViewPosition.getZoomLevel()>20) {
            mapView.getModel().mapViewPosition.setZoomLevel((byte) 20);
        }
    }

    public void selectCityMap(int cityIndex)
    {
        // TODO: Überprüfen, ob korrekt
        //Log.d(LOG_TAG, "rotateMap rotationAngle=" + rotationAngle);
        // mapView.setRotation(mapView.getRotation() + rotationAngle);

            switch (cityIndex) {
                case 0:
                    mapView.getModel().mapViewPosition.setCenter(new LatLong(48.3705449, 10.897789999999986));
                    mapView.getModel().mapViewPosition.setZoomLevel((byte) 14);
                    break; /*Augsburg*/
                case 1:
                    mapView.getModel().mapViewPosition.setCenter(new LatLong(49.8988135, 10.902763600000071));
                    mapView.getModel().mapViewPosition.setZoomLevel((byte) 14);
                    break; /*Bamberg*/
                case 2:
                    mapView.getModel().mapViewPosition.setCenter(new LatLong(49.9456399, 11.5713346));
                    mapView.getModel().mapViewPosition.setZoomLevel((byte) 14);
                    break; /*Bayreuth*/
                case 3:
                    mapView.getModel().mapViewPosition.setCenter(new LatLong(49.5896744, 11.011961100000008));
                    mapView.getModel().mapViewPosition.setZoomLevel((byte) 14);
                    break; /*Erlangen*/
                case 4:
                    mapView.getModel().mapViewPosition.setCenter(new LatLong(47.569648, 10.700432800000044));
                    mapView.getModel().mapViewPosition.setZoomLevel((byte) 14);
                    break; /*Fuessen*/
                case 5:
                    mapView.getModel().mapViewPosition.setCenter(new LatLong(48.7665351, 11.42575409999995));
                    mapView.getModel().mapViewPosition.setZoomLevel((byte) 14);
                    break; /*Ingolstadt*/
                case 6:
                    mapView.getModel().mapViewPosition.setCenter(new LatLong(47.7285699, 10.315783499999952));
                    mapView.getModel().mapViewPosition.setZoomLevel((byte) 14);
                    break; /*Kempten*/
                case 7:
                    mapView.getModel().mapViewPosition.setCenter(new LatLong(48.5441917, 12.14685320000001));
                    mapView.getModel().mapViewPosition.setZoomLevel((byte) 14);
                    break; /*Landshut*/
                case 8:
                    mapView.getModel().mapViewPosition.setCenter(new LatLong(48.1351253, 11.581980599999952));
                    mapView.getModel().mapViewPosition.setZoomLevel((byte) 13);
                    break; /*Muenchen*/
                case 9:
                    mapView.getModel().mapViewPosition.setCenter(new LatLong(49.45203, 11.076749999999947));
                    mapView.getModel().mapViewPosition.setZoomLevel((byte) 14);
                    break; /*Nuernberg*/
                case 10:
                    mapView.getModel().mapViewPosition.setCenter(new LatLong(48.5667364, 13.431946599999947));
                    mapView.getModel().mapViewPosition.setZoomLevel((byte) 14);
                    break; /*Passau*/
                case 11:
                    mapView.getModel().mapViewPosition.setCenter(new LatLong(49.0134297, 12.101623600000039));
                    mapView.getModel().mapViewPosition.setZoomLevel((byte) 14);
                    break; /*Regensburg*/
                case 12:
                    mapView.getModel().mapViewPosition.setCenter(new LatLong(47.8571272, 12.118104700000004));
                    mapView.getModel().mapViewPosition.setZoomLevel((byte) 14);
                    break; /*Rosenheim*/
                case 13:
                    mapView.getModel().mapViewPosition.setCenter(new LatLong(48.4010822, 9.987607600000047));
                    mapView.getModel().mapViewPosition.setZoomLevel((byte) 14);
                    break; /*Ulm*/
                case 14:
                    mapView.getModel().mapViewPosition.setCenter(new LatLong(49.79130439999999, 9.953354799999943));
                    mapView.getModel().mapViewPosition.setZoomLevel((byte) 14);
                    break; /*Würzburg*/
                case 15:
                    mapView.getModel().mapViewPosition.setCenter(new LatLong(48.1509834021902, 11.56798113464049));
                    mapView.getModel().mapViewPosition.setZoomLevel((byte) 18);
                    break; /*eCARus-Keller*/
                default:
                    break;
            }

    }
}
