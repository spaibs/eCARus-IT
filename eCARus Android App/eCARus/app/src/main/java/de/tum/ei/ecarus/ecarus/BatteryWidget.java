package de.tum.ei.ecarus.ecarus;

import android.content.Context;
import android.content.res.Resources;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.util.AttributeSet;
import android.util.TypedValue;
import android.view.View;

/****************************
 * The BatteryWidget class defines a widget that displays
 * the battery level set by the function setBatteryLevel called in the MainActivity.
 * From 0-33% battery level the color of the bar is red,
 * from 33-65% battery level the color of the bar is yellow,
 * from 66-100% battery level the color of the bar is green.
 */

public class BatteryWidget extends View {

    private Paint paint;
    // widget size
    public int width;
    public int height;
    public int batteryLevel;

    // values starting with px take the pixel density of the display into account
    private float pxwidth;
    private float pxheight;
    private float px4;
    // size of the text
    private float pxsize;

    public BatteryWidget(Context context, AttributeSet attrs) {
        super(context, attrs);
        paint = new Paint();
        width = 200;
        height = 50;
        batteryLevel = 0;

        // converting values
        Resources r = getResources();
        pxwidth = TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_DIP, width, r.getDisplayMetrics());
        pxheight = TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_DIP, height, r.getDisplayMetrics());
        px4 = TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_DIP, 4, r.getDisplayMetrics());
        pxsize = TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_DIP, 30, r.getDisplayMetrics());
    }

    // set dimensions of the widget according to the pixel density
    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        setMeasuredDimension((int) pxwidth, (int) pxheight);
    }

    // draw the widget
    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);

        //draw the big black rectangle
        paint.setColor(Color.BLACK);
        canvas.drawRect(0f, 0f, pxwidth, pxheight, paint);

        //adapt color according to the battery level
        if (batteryLevel < 34) {
            //red
            paint.setColor(Color.rgb(255, 61, 0));
        } else if (batteryLevel < 65) {
            //yellow
            paint.setColor(Color.rgb(255, 255, 59));
        } else {
            //green
            paint.setColor(Color.rgb(76, 175, 80));
        }

        //draw small rectangle according to the battery level
        if (batteryLevel < 0) {
            canvas.drawRect(px4, px4, -px4, pxheight - px4, paint);
        }
        else if (batteryLevel > 100){
            canvas.drawRect(px4, px4, pxwidth - px4, pxheight - px4, paint);
        }
        else {
            canvas.drawRect(px4, px4, pxwidth / 100 * batteryLevel - px4, pxheight - px4, paint);
        }

        //set text settings
        paint.setColor(Color.WHITE);
        paint.setTextSize(pxsize);
        paint.setShadowLayer(10.0f, 2.0f, 2.0f, Color.BLACK);

        //draw text according to the battery level
        if (batteryLevel < 0) {
            canvas.drawText("0%", 20 * px4, 9 * px4, paint);
        }
        else if (batteryLevel < 10) {
            canvas.drawText(batteryLevel + "%", 20 * px4, 9 * px4, paint);
        }
        else if (batteryLevel < 100) {
            canvas.drawText(batteryLevel + "%", 19 * px4, 9 * px4, paint);
        }
        else {
            canvas.drawText("100%", 17 * px4, 9 * px4, paint);
        }
    }

    public void setBatteryLevel(int level) {
        batteryLevel = level;
    }
}