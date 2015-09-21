package com.example.gtflauncher.navigation;

import android.app.Activity;
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

    private MapView mapView;
    private TileCache tileCache;
    private TileRendererLayer tileRendererLayer;
    private File mapFile;

    private final Activity appActivity;

    private boolean navigationStarted = false;

    public NavigationService(final Activity appActivity, final String modelPath)
    {
        this.appActivity = appActivity;

        //See: http://ftp-stud.hs-esslingen.de/pub/Mirrors/download.mapsforge.org/maps/
        mapFile = new File(modelPath + "/maps/", "germany.map");
        if(!mapFile.isFile())
        {
            Log.e("NavigationService", "mapFile '" + mapFile.getAbsoluteFile() + "' couldn't be found.");
        }

        NavigationJNIAccess.getInstance().registerNavigationService(this);
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

        // 48.779209, 9.1772152 = Stuttgart
        mapView.getModel().mapViewPosition.setCenter(new LatLong(48.779209, 9.1772152));
        mapView.getModel().mapViewPosition.setZoomLevel((byte) 12);

        // tile renderer layer using internal render theme
        MapDataStore mapDataStore = new MapFile(mapFile);
        tileRendererLayer = new TileRendererLayer(tileCache, mapDataStore,
                mapView.getModel().mapViewPosition, false, true, AndroidGraphicFactory.INSTANCE);
        tileRendererLayer.setXmlRenderTheme(InternalRenderTheme.OSMARENDER);

        // only once a layer is associated with a mapView the rendering starts
        mapView.getLayerManager().getLayers().add(tileRendererLayer);

        navigationStarted = true;
    }

    public void shutdownNavigation()
    {
        mapView.destroyAll();
    }

    public void scrollMapCenter(int deltaX, int deltaY)
    {
        if(!navigationStarted)
        {
            startNavigation();
        }
        mapView.getModel().mapViewPosition.moveCenter(deltaX, deltaY);
    }

    public void rotateMap(float rotationAngle)
    {
        // TODO: Überprüfen, ob korrekt
        Log.d(LOG_TAG, "rotateMap rotationAngle=" + rotationAngle);
        //mapView.setRotation(mapView.getRotation() + rotationAngle);
    }

    public void zoomMap(float zoomFactor)
    {
        // TODO: Überprüfen, ob korrekt
        Log.d(LOG_TAG, "zoomMap zoomFactor=" + zoomFactor);
        /*if(zoomFactor > 0.0f)
        {
            mapView.getModel().mapViewPosition.zoomOut();
        }
        else
        {
            mapView.getModel().mapViewPosition.zoomIn();
        }*/
    }
}
