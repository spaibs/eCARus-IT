package de.tum.ei.ecarus.test2;

import android.content.Context;
import android.content.res.Resources;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.util.AttributeSet;
import android.util.TypedValue;
import android.view.View;

public class BatteryWidget extends View {

    private Paint paint;
    private Paint stkpaint;
    // widget size
    public int width;
    public int height;
    public int batteryLevel;
    float px4;
    float px1;
    float pxwidth;
    float pxheight;
    float pxsize;

    public BatteryWidget(Context context, AttributeSet attrs) {
        super(context, attrs);
        makePaint();
        width = 200;
        height = 50;
        batteryLevel = 50;

        Resources r = getResources();
        px4 = TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_DIP, 4, r.getDisplayMetrics());
        pxwidth = TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_DIP, width, r.getDisplayMetrics());
        pxheight = TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_DIP, height, r.getDisplayMetrics());
        pxsize = TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_DIP, 30, r.getDisplayMetrics());
    }

    private void makePaint() {
        paint = new Paint();
        stkpaint = new Paint();
        stkpaint.setStyle(Paint.Style.STROKE);
        stkpaint.setStrokeWidth(8);
        stkpaint.setColor(Color.GRAY);
    }

    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        setMeasuredDimension((int) pxwidth, (int) pxheight);
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);


        paint.setColor(Color.BLACK);
        canvas.drawRect(0f, 0f, pxwidth, pxheight, paint);

        if (batteryLevel < 34) {
            paint.setColor(Color.rgb(255, 61, 0));
        }
        else if (batteryLevel < 65){
            paint.setColor(Color.rgb(255, 255, 59));
        }
        else {
            paint.setColor(Color.rgb(76, 175, 80));
        }

        canvas.drawRect(px4, px4, pxwidth / 100 * batteryLevel - px4, pxheight - px4, paint);
        paint.setColor(Color.WHITE);
        paint.setTextSize(pxsize);

        if (batteryLevel < 10) {
            canvas.drawText(batteryLevel + "%", 20 * px4, 9 * px4, paint);
            canvas.drawText(batteryLevel + "%", 20 * px4, 9 * px4, stkpaint);
        }
        else if (batteryLevel < 100) {
            canvas.drawText(batteryLevel + "%", 19 * px4, 9 * px4, paint);
            canvas.drawText(batteryLevel + "%", 19 * px4, 9 * px4, stkpaint);
        }
        else {
            canvas.drawText(batteryLevel + "%", 17 * px4, 9 * px4, paint);
            canvas.drawText(batteryLevel + "%", 17 * px4, 9 * px4, paint);
        }

    }
}
