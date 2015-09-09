package com.example.displaysocketservice;

import java.util.Arrays;
import java.util.Enumeration;
import java.util.Hashtable;
import java.util.Locale;

import android.util.Log;

public class MainManager {

private static boolean New_light_status_message = false;
private static boolean New_gesture_message = false;
private static boolean New_bike_message_totower = false;
private static boolean New_bike_message_fromtower = false;

static boolean  datastructure_is_not_accessed = true;

static Hashtable<String, byte[]> actualBusValues = new Hashtable<String, byte[]>(1000, (float) 0.75);

private static boolean 	GUIUptadeRequired=false,
						FernLicht=false,NormLicht=false,
						BlinkerRechts=false,BlinkerLinks=false;

private static byte 	AutoModi=0b00000000;

private static boolean 	TowerBMSRequest=false;
private static int 		UpdatedCellCount=0;

private static int 		Strom=0,StackSpannung=0,HochsteSpannungZelle=0,
						NiedrigsteSpannungZelle=0,MaxTemperature=0;
private static String 	StringStrom = null,StringStackSpannung = null,StringHochsteSpannungZelle = null,
						StringNiedrigsteSpannungZelle = null,StringMaxTemperature = null;

private static int 		SpannungsZelle0 = 0,SpannungsZelle1 = 0,SpannungsZelle2 = 0,SpannungsZelle3 = 0,
 						SpannungsZelle4 = 0,SpannungsZelle5 = 0,SpannungsZelle6 = 0,SpannungsZelle7 = 0,
 						SpannungsZelle8 = 0,SpannungsZelle9 = 0,SpannungsZelle10 = 0,SpannungsZelle11 = 0,
 						SpannungsZelle12 = 0,SpannungsZelle13= 0,SpannungsZelle14 = 0,SpannungsZelle15 = 0;
private static String 	StringSpannungsZelle0 = null,StringSpannungsZelle1 = null,StringSpannungsZelle2= null,
						StringSpannungsZelle3 = null,StringSpannungsZelle4 = null,StringSpannungsZelle5 = null,
						StringSpannungsZelle6 = null,StringSpannungsZelle7 = null,StringSpannungsZelle8 = null,
						StringSpannungsZelle9 = null,StringSpannungsZelle10 = null,StringSpannungsZelle11 = null,
						StringSpannungsZelle12 = null,StringSpannungsZelle13 = null,StringSpannungsZelle14 = null,
						StringSpannungsZelle15 = null;

private static String 	sKey251,sKey252,sKey300,sKey419,sKey453,sKey454,sKey455,sKey456;

public static void main( final byte [] key, final byte [] value){

    //Log.w("MainManager", "Key and Info" + Arrays.toString(key) + Arrays.toString(value)); //TODO

    if(key[0] == 102 && key[1] == 6) { // BIKE MESSAGES 0x666 FROM TOWER
        Bike.handleIncomingFromTower(value);
    }

    if(key[0] == 103 && key[1] == 6) { // BIKE MESSAGES 0x667 TO TOWER
        Bike.handleIncomingToTower(value);
    }

    if(key[0] == 1 && key[1] == 4){ // GESTURE MESSAGES 0x401
        Gestures.handleIncoming(value);
    }

    if(key[0] == 0 && key[1] == 3){ // LIGHT STATE MESSAGES 0x300   // TODO ADD ONE MORE ZERO HERE OR NOT

        LightStates.handleIncoming(value);

        /*while(LightStates.isAccessed()){
            Log.w("MainManager", "MainManager.main() waiting for LightStates access permission.");
        }

        LightStates.setAccessed(true);*/
        /*if( (value[2]>>5 & 1) == 1){ // TODO FUNKTIONEN AUSLAGERN FUER READABILITY

            if((value[1]>>5 & 1) == 1){
                LightStates.setFernLicht(true);
            } else LightStates.setFernLicht(false);

            if((value[1]>>4 & 1) == 1){
                LightStates.setNormLicht(true);
            } else LightStates.setNormLicht(false);

        }*/

       /* if( (value[2]>>5 & 1) == 1 && (value[1]>>5 & 1) == 1){ // TODO FUNKTIONEN AUSLAGERN FUER READABILITY ??
            LightStates.setFernLicht(true);
        } else LightStates.setFernLicht(false);

        if( (value[2]>>5 & 1) == 1 && (value[1]>>4 & 1) == 1){
            LightStates.setNormLicht(true);
        } else LightStates.setNormLicht(false);

        if( (value[0]>>1 & 1) == 1 && (value[2]>>1 & 1) == 1){
            LightStates.setBlinkerRechts(true);
        } else LightStates.setBlinkerRechts(false);

        if( (value[0] & 1) == 1 && (value[2] & 1) == 1){
            LightStates.setBlinkerLinks(true);
        } else LightStates.setBlinkerLinks(false);


        LightStates.setAccessed(false);
        MainManager.setNew_light_status_message(true);*/

    }// else {
     //   Log.w("MainManager", "ELSE Key and Info" + Arrays.toString(key) + Arrays.toString(value));
    //}
		
		/*try{
			
			if(isMessageValid(key)){

              if(key[0] == 0 && key[1] == 3){
                  Log.w("MainManager", "Key and Info" + Arrays.toString(key) + Arrays.toString(value));
              }
						
			  String tempKey = new String(key);
			  
			  //Log.w("MainManager", "Key and Info" + Arrays.toString(key) + Arrays.toString(value));	// TODO


              while(!datastructure_is_not_accessed){
                  Log.w("MainManager", "MainManager WAITING FOR TOKEN (want to PUT)");
              }

              datastructure_is_not_accessed = false;

			  actualBusValues.put(tempKey, value);

              datastructure_is_not_accessed = true;
			  
			  //Log.w("MainManager", " Table Size: " + actualBusValues.size()); //TODO

			}
			
			else
			{
				Log.w("MainManager", "CAN-Message is not valid" + Arrays.toString(key));
			}
			
			// to print out all the elements of the Hashtable
			*//*
			Enumeration <String> e =actualBusValues.keys();
			  while(e.hasMoreElements()){
				  String param = (String) e.nextElement();
				  Log.w("MainManager", "\n\n Complete List: " + Arrays.toString(param.getBytes()) + "/" + Arrays.toString(actualBusValues.get(param)) + actualBusValues.size()+"\n");
			}
			*//*
			
		}
		
		catch(Exception e){
			
			Log.w("MainManager","Put List Error: " + e);
			
		};
	
		 try{ 
			 
			 if(!actualBusValues.isEmpty()){
				 
				 registerData();
				 
			 }
			 
	   }
		catch(Exception e){
			
			Log.w("MainManager","Register Data Error: " + e);
			
			}*/

}

public static void init(){
	
	byte[] tempKey = new byte[2];		//generating string Keys for HashTable
	
		tempKey[0] = 0x51;
		tempKey[1] = 0x02;
		sKey251 = new String(tempKey);
	
		tempKey[0] = 0x52;
		tempKey[1] = 0x02;
		sKey252 = new String(tempKey);
	
		tempKey[0] = 0x00;				
		tempKey[1] = 0x03;
		sKey300 = new String(tempKey);
		
		tempKey[0] = 0x19;
		tempKey[1] = 0x04;
		sKey419 = new String(tempKey);
	
		tempKey[0] = 0x53;
		tempKey[1] = 0x04;
		sKey453 = new String(tempKey);
	
		tempKey[0] = 0x54;
		tempKey[1] = 0x04;
		sKey454 = new String(tempKey);
	
		tempKey[0] = 0x55;
		tempKey[1] = 0x04;
		sKey455 = new String(tempKey);
	
		tempKey[0] = 0x56;
		tempKey[1] = 0x04;
		sKey456 = new String(tempKey);
}

static boolean isMessageValid(byte[] incoming)	//bytes are cast to int and then checked if they are valid IDs according to CAN-IDs documentation
{
	
	if((int)incoming[1]==1){													  //CAN-IDs mit 0x1XX
		 if((int)incoming[0]==80 	|| (int)incoming[0]==81 	||
			(int)incoming[0]==103	|| (int)incoming[0]==112	||
			(int)incoming[0]==-128	|| (int)incoming[0]==-127	||
			(int)incoming[0]==-112)
		 							{/*Log.w("MainManager", "Valid ID 1xx");*/return true;}} //TODO
	
	if((int)incoming[1]==2){													 //CAN-IDs mit 0x2XX
		 if((int)incoming[0]==34	|| 	(int)incoming[0]==36 	||
			(int)incoming[0]==80	|| 	(int)incoming[0]==81 	|| 
			(int)incoming[0]==82 	|| 	(int)incoming[0]==84 	|| 
			(int)incoming[0]==85 	|| 	(int)incoming[0]==-128	||
			(int)incoming[0]==-127	||	(int)incoming[0]==-112)
									{/*Log.w("MainManager", "Valid ID 2xx");*/return true;}} //TODO
	
	if((int)incoming[1]==3){													 //CAN-IDs mit 0x3XX
		if((int)incoming[0]==0)
									{/*Log.w("MainManager", "Valid ID 3xx");*/return true;}} //TODO
	
	if((int)incoming[1]==4){													 //CAN-IDs mit 0x4XX
		 if((int)incoming[0]==0 	|| 	(int)incoming[0]==2		||
			(int)incoming[0]==25 	||	(int)incoming[0]==32 	|| 
			(int)incoming[0]==83 	||	(int)incoming[0]==84 	|| 
			(int)incoming[0]==85 	||	(int)incoming[0]==86)
									{/*Log.w("MainManager", "Valid ID 4xx");*/return true;}} //TODO
	
	if((int)incoming[1]==5){													 //CAN-IDs mit 0x5XX
		 if((int)incoming[0]==-94 	|| 	(int)incoming[0]==-92 ||
			(int)incoming[0]==-91)
									{/*Log.w("MainManager", "Valid ID 5xx");*/return true;} //TODO
	}

	return false;
		
}


static void registerData(){
	
	//Log.w("MainManager", "Registering data.."); //TODO

	byte[] tempValue;
	
	boolean tempFernLicht = false,		tempNormLicht = false;
	boolean tempBlinkerRechts = false,	tempBlinkerLinks = false;
				
		if(actualBusValues.containsKey(sKey300)){

            while(!datastructure_is_not_accessed){
                Log.w("MainManager", "MainManager WAITING FOR TOKEN (want to GET)");
            }

            datastructure_is_not_accessed = false;

			tempValue = actualBusValues.get(sKey300);

            datastructure_is_not_accessed = true;
			
			//Log.w("MainManager", "ID 300, got: " + Arrays.toString(tempValue));			//TODO
			
			//TODO control values are wrong
			
			if( (tempValue[2]>>5 & 1) == 1){
				
				if((tempValue[1]>>5 & 1) == 1){ 
					tempFernLicht = true;
				}
				
				if((tempValue[1]>>4 & 1) == 1){
					tempNormLicht = true;
				}
				
			}
			
			if( (tempValue[0]>>1 & 1) == 1){
				if((tempValue[2]>>1 & 1) == 1){
					tempBlinkerRechts = true;
				}
			}
			
			if( (tempValue[0] & 1) == 1){
				if((tempValue[2] & 1) == 1){
					tempBlinkerLinks = true;
				}
			}
			
			setGUIUptadeRequired(checkForGUIUpdates(tempNormLicht, tempFernLicht, tempBlinkerLinks, tempBlinkerRechts));
		}
		
			if(actualBusValues.containsKey(sKey251)){

                while(!datastructure_is_not_accessed){
                    Log.w("MainManager", "MainManager WAITING FOR TOKEN (want to GET)");
                }

                datastructure_is_not_accessed = false;

                tempValue = actualBusValues.get(sKey251);

                datastructure_is_not_accessed = true;
				
				//Log.w("MainManager", "ID 251, got: " + Arrays.toString(tempValue)); //TODO
				
				int temp;
				
				temp = 0xFF00 & (tempValue[0]<<8);
				setStrom(temp);
				temp = 0xFF & tempValue[1];
				setStrom(getStrom() | temp);
				setStringStrom(String.format("%04d", getStrom()/10) + 
						"," + String.format("%01d", getStrom()%10));
							
				temp = 0xFF00 & (tempValue[2]<<8);
				setStackSpannung(temp);
				temp = 0xFF & tempValue[3];
				setStackSpannung(getStackSpannung() | temp);
				setStringStackSpannung(String.format("%02d", getStackSpannung()/1000) + 
						"," + String.format("%03d",getStackSpannung()%1000));
				
				temp = 0xFF00 & (tempValue[4]<<8);
				setHochsteSpannungZelle(temp);
				temp = 0xFF & tempValue[5];
				setHochsteSpannungZelle(getHochsteSpannungZelle() | temp);
				setStringHochsteSpannungZelle(String.format("%02d", getHochsteSpannungZelle()/1000) + 
						"," + String.format("%03d", getHochsteSpannungZelle()%1000));
				
				temp = 0xFF00 & (tempValue[6]<<8);
				setNiedrigsteSpannungZelle(temp);
				temp = 0xFF & tempValue[7];
				setNiedrigsteSpannungZelle(getNiedrigsteSpannungZelle() | temp);
				setStringNiedrigsteSpannungZelle(String.format("%02d", getNiedrigsteSpannungZelle()/1000) + 
						"," + String.format("%03d", getNiedrigsteSpannungZelle()%1000));
				
			}
			
			if(actualBusValues.containsKey(sKey252)){

                while(!datastructure_is_not_accessed){
                    Log.w("MainManager", "MainManager WAITING FOR TOKEN (want to GET)");
                }

                datastructure_is_not_accessed = false;

                tempValue = actualBusValues.get(sKey252);

                datastructure_is_not_accessed = true;
				
				//Log.w("MainManager", "ID 252, got: " + Arrays.toString(tempValue)); //TODO
				
				setMaxTemperature(tempValue[0]);
				setStringMaxTemperature(String.format(Locale.GERMANY, "%06d",getMaxTemperature()));
			}
			
			if(actualBusValues.containsKey(sKey419)){ // BMS Request

                while(!datastructure_is_not_accessed){
                    Log.w("MainManager", "MainManager WAITING FOR TOKEN (want to GET)");
                }

                datastructure_is_not_accessed = false;


                tempValue = actualBusValues.get(sKey419);

                datastructure_is_not_accessed = true;
				
				//Log.w("MainManager", "ID 419, got: " + Arrays.toString(tempValue)); //TODO
				
				if((int)tempValue[0]==2){
					
					setTowerBMSRequest(true);
				}
				
				setAutoModi((byte) 0b00000000);
				
				if((int)tempValue[1]==2){
				setAutoModi((byte) (getAutoModi()|0b00000100));	 
				}
					
				if((int)tempValue[2]==2){
				setAutoModi((byte) (getAutoModi()|0b00000010));	 
				}
				
				if((int)tempValue[3]==2){		
				setAutoModi((byte) (getAutoModi()|0b00000001));		
				}

			}
			
			if(actualBusValues.containsKey(sKey453)){ // 453 - 456 BMS DATA

                while(!datastructure_is_not_accessed){
                    Log.w("MainManager", "MainManager WAITING FOR TOKEN (want to GET)");
                }

                datastructure_is_not_accessed = false;

                tempValue = actualBusValues.get(sKey453);

                datastructure_is_not_accessed = true;
				
				// Log.w("MainManager", "ID 453, got: " + Arrays.toString(tempValue)); //TODO
				
				int temp = 0;
				String charTemp = "0";
				
				temp = 0xFF00 & (tempValue[0]<<8);
				SpannungsZelle0 = temp;
				temp = 0xFF & tempValue[1];
				SpannungsZelle0 |= temp;
				setStringSpannungsZelle0(String.format("%02d", SpannungsZelle0/1000) + "," + String.format("%03d",SpannungsZelle0%1000));
					
					if(SpannungsZelle0 < 300) charTemp = "1"; 									//empty
					else if(SpannungsZelle0 >= 300 && SpannungsZelle0 < 3000) charTemp = "2"; 	//low
					else if(SpannungsZelle0 >= 3000 && SpannungsZelle0 < 3500) charTemp = "3"; 	//mid
					else if(SpannungsZelle0 >= 3500) charTemp = "4"; 							//high
					setStringSpannungsZelle0(getStringSpannungsZelle0()
							+ charTemp);										    
			
				temp = 0xFF00 & (tempValue[2]<<8);
				SpannungsZelle1 = temp;
				temp = 0xFF & tempValue[3];
				SpannungsZelle1 |= temp;
				setStringSpannungsZelle1(String.format("%02d", SpannungsZelle1/1000) + "," + String.format("%03d",SpannungsZelle1%1000));
						
					if(SpannungsZelle1 < 300) charTemp = "1"; 									//empty
					else if(SpannungsZelle1 >= 300 && SpannungsZelle1 < 3000) charTemp = "2"; 	//low
					else if(SpannungsZelle1 >= 3000 && SpannungsZelle1 < 3500) charTemp = "3"; 	//mid
					else if(SpannungsZelle1>= 3500) charTemp = "4"; 							//high
					setStringSpannungsZelle1(getStringSpannungsZelle1()
							+ charTemp);
					
				temp = 0xFF00 & (tempValue[4]<<8);
				SpannungsZelle2 = temp;
				temp = 0xFF & tempValue[5];
				SpannungsZelle2 |= temp;
				setStringSpannungsZelle2(String.format("%02d", SpannungsZelle2/1000) + "," + String.format("%03d",SpannungsZelle2%1000));
					if(SpannungsZelle2 < 300) charTemp = "1"; 									//empty
					else if(SpannungsZelle2 >= 300 && SpannungsZelle2 < 3000) charTemp = "2"; 	//low
					else if(SpannungsZelle2 >= 3000 && SpannungsZelle2 < 3500) charTemp = "3"; 	//mid
					else if(SpannungsZelle2 >= 3500) charTemp = "4"; 							//high
					setStringSpannungsZelle2(getStringSpannungsZelle2()
							+ charTemp);
			
				temp = 0xFF00 & (tempValue[6]<<8);
				SpannungsZelle3 = temp;
				temp = 0xFF & tempValue[7];
				SpannungsZelle3 |= temp;
				setStringSpannungsZelle3(String.format("%02d", SpannungsZelle3/1000) + "," + String.format("%03d",SpannungsZelle3%1000));
					if(SpannungsZelle3 < 300) charTemp = "1"; 									//empty
					else if(SpannungsZelle3 >= 300 && SpannungsZelle3 < 3000) charTemp = "2"; 	//low
					else if(SpannungsZelle3 >= 3000 && SpannungsZelle3 < 3500) charTemp = "3"; 	//mid
					else if(SpannungsZelle3 >= 3500) charTemp = "4"; 							//high
					setStringSpannungsZelle3(getStringSpannungsZelle3()
							+ charTemp);
					
					setUpdatedCellCount(getUpdatedCellCount()+4);
			}
			
			if(actualBusValues.containsKey(sKey454)){

                while(!datastructure_is_not_accessed){
                    Log.w("MainManager", "MainManager WAITING FOR TOKEN (want to GET)");
                }

                datastructure_is_not_accessed = false;

                tempValue = actualBusValues.get(sKey454);

                datastructure_is_not_accessed = true;
				//Log.w("MainManager", "ID 454 got: " + Arrays.toString(tempValue));//TODO
				
				int temp = 0;
				String charTemp = "0";
				
				temp = 0xFF00 & (tempValue[0]<<8);
				SpannungsZelle4 = temp;
				temp = 0xFF & tempValue[1];
				SpannungsZelle4 |= temp;
				setStringSpannungsZelle4(String.format("%02d", SpannungsZelle4/1000) + "," + String.format("%03d",SpannungsZelle4%1000));
					
					if(SpannungsZelle4 < 300) charTemp = "1"; 									//empty
					else if(SpannungsZelle4 >= 300 && SpannungsZelle4 < 3000) charTemp = "2"; 	//low
					else if(SpannungsZelle4 >= 3000 && SpannungsZelle4 < 3500) charTemp = "3"; 	//mid
					else if(SpannungsZelle4 >= 3500) charTemp = "4"; 							//high
					setStringSpannungsZelle4(getStringSpannungsZelle4()
							+ charTemp);										    
			
				temp = 0xFF00 & (tempValue[2]<<8);
				SpannungsZelle5 = temp;
				temp = 0xFF & tempValue[3];
				SpannungsZelle5 |= temp;
				setStringSpannungsZelle5(String.format("%02d", SpannungsZelle5/1000) + "," + String.format("%03d",SpannungsZelle5%1000));
						
					if(SpannungsZelle5 < 300) charTemp = "1"; 									//empty
					else if(SpannungsZelle5 >= 300 && SpannungsZelle5 < 3000) charTemp = "2"; 	//low
					else if(SpannungsZelle5 >= 3000 && SpannungsZelle5 < 3500) charTemp = "3"; 	//mid
					else if(SpannungsZelle5>= 3500) charTemp = "4"; 							//high
					setStringSpannungsZelle5(getStringSpannungsZelle5()
							+ charTemp);
					
				temp = 0xFF00 & (tempValue[4]<<8);
				SpannungsZelle6 = temp;
				temp = 0xFF & tempValue[5];
				SpannungsZelle6 |= temp;
				setStringSpannungsZelle6(String.format("%02d", SpannungsZelle6/1000) + "," + String.format("%03d",SpannungsZelle6%1000));
					if(SpannungsZelle6 < 300) charTemp = "1"; 									//empty
					else if(SpannungsZelle6 >= 300 && SpannungsZelle6 < 3000) charTemp = "2"; 	//low
					else if(SpannungsZelle6 >= 3000 && SpannungsZelle6< 3500) charTemp = "3"; 	//mid
					else if(SpannungsZelle6 >= 3500) charTemp = "4"; 							//high
					setStringSpannungsZelle6(getStringSpannungsZelle6()
							+ charTemp);
			
				temp = 0xFF00 & (tempValue[6]<<8);
				SpannungsZelle7 = temp;
				temp = 0xFF & tempValue[7];
				SpannungsZelle7 |= temp;
				setStringSpannungsZelle7(String.format("%02d", SpannungsZelle7/1000) + "," + String.format("%03d",SpannungsZelle7%1000));
					if(SpannungsZelle7 < 300) charTemp = "1"; 									//empty
					else if(SpannungsZelle7 >= 300 && SpannungsZelle7 < 3000) charTemp = "2"; 	//low
					else if(SpannungsZelle7 >= 3000 && SpannungsZelle7 < 3500) charTemp = "3"; 	//mid
					else if(SpannungsZelle7 >= 3500) charTemp = "4"; 							//high
					setStringSpannungsZelle7(getStringSpannungsZelle7()
							+ charTemp);	
					
					setUpdatedCellCount(getUpdatedCellCount()+4);
			}
			
			if(actualBusValues.containsKey(sKey455)){

                while(!datastructure_is_not_accessed){
                    Log.w("MainManager", "MainManager WAITING FOR TOKEN (want to GET)");
                }

                datastructure_is_not_accessed = false;

                tempValue = actualBusValues.get(sKey455);

                datastructure_is_not_accessed = true;

				//Log.w("MainManager", "ID 455 got: " + Arrays.toString(tempValue));//TODO
				
				int temp = 0;
				String charTemp = "0";
				
				temp = 0xFF00 & (tempValue[0]<<8);
				SpannungsZelle8 = temp;
				temp = 0xFF & tempValue[1];
				SpannungsZelle8 |= temp;
				setStringSpannungsZelle8(String.format("%02d", SpannungsZelle8/1000) + "," + String.format("%03d",SpannungsZelle8%1000));
					
					if(SpannungsZelle8 < 300) charTemp = "1"; 									//empty
					else if(SpannungsZelle8 >= 300 && SpannungsZelle8 < 3000) charTemp = "2"; 	//low
					else if(SpannungsZelle8 >= 3000 && SpannungsZelle8 < 3500) charTemp = "3"; 	//mid
					else if(SpannungsZelle8 >= 3500) charTemp = "4"; 							//high
					setStringSpannungsZelle8(getStringSpannungsZelle8()
							+ charTemp);										    
			
				temp = 0xFF00 & (tempValue[2]<<8);
				SpannungsZelle9 = temp;
				temp = 0xFF & tempValue[3];
				SpannungsZelle9 |= temp;
				setStringSpannungsZelle9(String.format("%02d", SpannungsZelle9/1000) + "," + String.format("%03d",SpannungsZelle9%1000));
						
					if(SpannungsZelle9 < 300) charTemp = "1"; 									//empty
					else if(SpannungsZelle9 >= 300 && SpannungsZelle9 < 3000) charTemp = "2"; 	//low
					else if(SpannungsZelle9 >= 3000 && SpannungsZelle9 < 3500) charTemp = "3"; 	//mid
					else if(SpannungsZelle9>= 3500) charTemp = "4"; 							//high
					setStringSpannungsZelle9(getStringSpannungsZelle9()
							+ charTemp);
					
				temp = 0xFF00 & (tempValue[4]<<8);
				SpannungsZelle10 = temp;
				temp = 0xFF & tempValue[5];
				SpannungsZelle10 |= temp;
				setStringSpannungsZelle10(String.format("%02d", SpannungsZelle10/1000) + "," + String.format("%03d",SpannungsZelle10%1000));
					if(SpannungsZelle10 < 300) charTemp = "1"; 									//empty
					else if(SpannungsZelle10 >= 300 && SpannungsZelle10 < 3000) charTemp = "2"; 	//low
					else if(SpannungsZelle10 >= 3000 && SpannungsZelle10< 3500) charTemp = "3"; 	//mid
					else if(SpannungsZelle10 >= 3500) charTemp = "4"; 							//high
					setStringSpannungsZelle10(getStringSpannungsZelle10()
							+ charTemp);
			
				temp = 0xFF00 & (tempValue[6]<<8);
				SpannungsZelle11 = temp;
				temp = 0xFF & tempValue[7];
				SpannungsZelle11 |= temp;
				setStringSpannungsZelle11(String.format("%02d", SpannungsZelle11/1000) + "," + String.format("%03d",SpannungsZelle11%1000));
					if(SpannungsZelle7 < 300) charTemp = "1"; 									//empty
					else if(SpannungsZelle11 >= 300 && SpannungsZelle11 < 3000) charTemp = "2"; 	//low
					else if(SpannungsZelle11 >= 3000 && SpannungsZelle11 < 3500) charTemp = "3"; 	//mid
					else if(SpannungsZelle11 >= 3500) charTemp = "4"; 							//high
					setStringSpannungsZelle11(getStringSpannungsZelle11()
							+ charTemp);		
					
					setUpdatedCellCount(getUpdatedCellCount()+4);
			
			}
			
			if(actualBusValues.containsKey(sKey456)){

                while(!datastructure_is_not_accessed){
                    Log.w("MainManager", "MainManager WAITING FOR TOKEN (want to GET)");
                }

                datastructure_is_not_accessed = false;

                tempValue = actualBusValues.get(sKey456);

                datastructure_is_not_accessed = true;

				//Log.w("MainManager", "ID 456 got: " + Arrays.toString(tempValue));//TODO
				
				int temp = 0;
				String charTemp = "0";
				
				temp = 0xFF00 & (tempValue[0]<<8);
				SpannungsZelle12 = temp;
				temp = 0xFF & tempValue[1];
				SpannungsZelle12 |= temp;
				setStringSpannungsZelle12(String.format("%02d", SpannungsZelle12/1000) + "," + String.format("%03d",SpannungsZelle12%1000));
					
					if(SpannungsZelle12 < 300) charTemp = "1"; 									//empty
					else if(SpannungsZelle12 >= 300 && SpannungsZelle12 < 3000) charTemp = "2"; 	//low
					else if(SpannungsZelle12 >= 3000 && SpannungsZelle12 < 3500) charTemp = "3"; 	//mid
					else if(SpannungsZelle12 >= 3500) charTemp = "4"; 							//high
					setStringSpannungsZelle12(getStringSpannungsZelle12()
							+ charTemp);										    
			
				temp = 0xFF00 & (tempValue[2]<<8);
				SpannungsZelle13 = temp;
				temp = 0xFF & tempValue[3];
				SpannungsZelle13 |= temp;
				setStringSpannungsZelle13(String.format("%02d", SpannungsZelle13/1000) + "," + String.format("%03d",SpannungsZelle13%1000));
						
					if(SpannungsZelle13 < 300) charTemp = "1"; 									//empty
					else if(SpannungsZelle13 >= 300 && SpannungsZelle13 < 3000) charTemp = "2"; 	//low
					else if(SpannungsZelle13 >= 3000 && SpannungsZelle13 < 3500) charTemp = "3"; 	//mid
					else if(SpannungsZelle13>= 3500) charTemp = "4"; 							//high
					setStringSpannungsZelle13(getStringSpannungsZelle13()
							+ charTemp);
					
				temp = 0xFF00 & (tempValue[4]<<8);
				SpannungsZelle14 = temp;
				temp = 0xFF & tempValue[5];
				SpannungsZelle14 |= temp;
				setStringSpannungsZelle14(String.format("%02d", SpannungsZelle14/1000) + "," + String.format("%03d",SpannungsZelle14%1000));
					if(SpannungsZelle10 < 300) charTemp = "1"; 									//empty
					else if(SpannungsZelle14 >= 300 && SpannungsZelle14 < 3000) charTemp = "2"; 	//low
					else if(SpannungsZelle14 >= 3000 && SpannungsZelle14< 3500) charTemp = "3"; 	//mid
					else if(SpannungsZelle14 >= 3500) charTemp = "4"; 							//high
					setStringSpannungsZelle14(getStringSpannungsZelle14()
							+ charTemp);
			
				temp = 0xFF00 & (tempValue[6]<<8);
				SpannungsZelle15 = temp;
				temp = 0xFF & tempValue[7];
				SpannungsZelle15 |= temp;
				setStringSpannungsZelle15(String.format("%02d", SpannungsZelle15/1000) + "," + String.format("%03d",SpannungsZelle15%1000));
					if(SpannungsZelle15 < 300) charTemp = "1"; 									//empty
					else if(SpannungsZelle15 >= 300 && SpannungsZelle15 < 3000) charTemp = "2"; 	//low
					else if(SpannungsZelle15 >= 3000 && SpannungsZelle15 < 3500) charTemp = "3"; 	//mid
					else if(SpannungsZelle15 >= 3500) charTemp = "4"; 							//high
					setStringSpannungsZelle15(getStringSpannungsZelle15()
							+ charTemp);
					
					setUpdatedCellCount(getUpdatedCellCount()+4);
			}

}

static boolean checkForGUIUpdates(boolean tempNormLicht, boolean tempFernLicht, boolean tempBlinkerLinks, boolean tempBlinkerRechts){
	
	boolean ChangeNeeded = false;
	
	if(tempNormLicht!=getNormLicht())
	{
		setNormLicht(tempNormLicht);
		ChangeNeeded = true;
	}
	if(tempFernLicht!=getFernLicht())
	{
		setFernLicht(tempFernLicht);
		ChangeNeeded = true;
	}
	if(tempBlinkerLinks!=getBlinkerLinks())
	{
		setBlinkerLinks(tempBlinkerLinks);
		ChangeNeeded = true;
	}
	if(tempBlinkerRechts!=getBlinkerRechts())
	{
		setBlinkerRechts(tempBlinkerRechts);
		ChangeNeeded = true;
	}
	
	return ChangeNeeded;
	
}

public static boolean getNormLicht() {
	return NormLicht;
}
public static void setNormLicht(boolean normLicht) {
	NormLicht = normLicht;
}
public static boolean getFernLicht() {
	return FernLicht;
}
public static void setFernLicht(boolean fernLicht) {
	FernLicht = fernLicht;
}
public static boolean getBlinkerLinks() {
	return BlinkerLinks;
}
public static void setBlinkerLinks(boolean blinkerLinks) {
	BlinkerLinks = blinkerLinks;
}
public static boolean getBlinkerRechts() {
	return BlinkerRechts;
}
public static void setBlinkerRechts(boolean blinkerRechts) {
	BlinkerRechts = blinkerRechts;
}
public static boolean getGUIUptadeRequired() {
	return GUIUptadeRequired;
}
public static void setGUIUptadeRequired(boolean gUIUptadeRequired) {
	GUIUptadeRequired = gUIUptadeRequired;
}
public static int getStrom() {
	return Strom;
}
public static void setStrom(int strom) {
	Strom = strom;
}
public static int getStackSpannung() {
	return StackSpannung;
}
public static void setStackSpannung(int stackSpannung) {
	StackSpannung = stackSpannung;
}
public static int getHochsteSpannungZelle() {
	return HochsteSpannungZelle;
}
public static void setHochsteSpannungZelle(int hochsteSpannungZelle) {
	HochsteSpannungZelle = hochsteSpannungZelle;
}
public static int getNiedrigsteSpannungZelle() {
	return NiedrigsteSpannungZelle;
}
public static void setNiedrigsteSpannungZelle(int niedrigsteSpannungZelle) {
	NiedrigsteSpannungZelle = niedrigsteSpannungZelle;
}
public static int getMaxTemperature() {
	return MaxTemperature;
}
public static void setMaxTemperature(byte maxTemperature) {
	MaxTemperature = maxTemperature;
}
public static String getStringStrom() {
	return StringStrom;
}
public static void setStringStrom(String stringStrom) {
	StringStrom = stringStrom;
}
public static String getStringStackSpannung() {
	return StringStackSpannung;
}
public static void setStringStackSpannung(String stringStackSpannung) {
	StringStackSpannung = stringStackSpannung;
}
public static String getStringHochsteSpannungZelle() {
	return StringHochsteSpannungZelle;
}
public static void setStringHochsteSpannungZelle(String stringHochsteSpannungZelle) {
	StringHochsteSpannungZelle = stringHochsteSpannungZelle;
}
public static String getStringNiedrigsteSpannungZelle() {
	return StringNiedrigsteSpannungZelle;
}
public static void setStringNiedrigsteSpannungZelle(String stringNiedrigsteSpannungZelle) {
	StringNiedrigsteSpannungZelle = stringNiedrigsteSpannungZelle;
}
public static String getStringMaxTemperature() {
	return StringMaxTemperature;
}
public static void setStringMaxTemperature(String stringMaxTemperature) {
	StringMaxTemperature = stringMaxTemperature;
}
public static boolean getTowerBMSRequest() {
	return TowerBMSRequest;
}
public static void setTowerBMSRequest(boolean towerBMSRequest) {
	TowerBMSRequest = towerBMSRequest;
}
public static byte getAutoModi() {
	return AutoModi;
}
public static void setAutoModi(byte autoModi) {
	AutoModi = autoModi;
}
public static String getStringSpannungsZelle2() {
	return StringSpannungsZelle2;
}
public static void setStringSpannungsZelle2(String stringSpannungsZelle2) {
	StringSpannungsZelle2 = stringSpannungsZelle2;
}
public static String getStringSpannungsZelle1() {
	return StringSpannungsZelle1;
}
public static void setStringSpannungsZelle1(String stringSpannungsZelle1) {
	StringSpannungsZelle1 = stringSpannungsZelle1;
}
public static String getStringSpannungsZelle0() {
	return StringSpannungsZelle0;
}
public static void setStringSpannungsZelle0(String stringSpannungsZelle0) {
	StringSpannungsZelle0 = stringSpannungsZelle0;
}
public static String getStringSpannungsZelle3() {
	return StringSpannungsZelle3;
}
public static void setStringSpannungsZelle3(String stringSpannungsZelle3) {
	StringSpannungsZelle3 = stringSpannungsZelle3;
}
public static String getStringSpannungsZelle4() {
	return StringSpannungsZelle4;
}
public static void setStringSpannungsZelle4(String stringSpannungsZelle4) {
	StringSpannungsZelle4 = stringSpannungsZelle4;
}
public static String getStringSpannungsZelle6() {
	return StringSpannungsZelle6;
}
public static void setStringSpannungsZelle6(String stringSpannungsZelle6) {
	StringSpannungsZelle6 = stringSpannungsZelle6;
}
public static String getStringSpannungsZelle5() {
	return StringSpannungsZelle5;
}
public static void setStringSpannungsZelle5(String stringSpannungsZelle5) {
	StringSpannungsZelle5 = stringSpannungsZelle5;
}
public static String getStringSpannungsZelle7() {
	return StringSpannungsZelle7;
}
public static void setStringSpannungsZelle7(String stringSpannungsZelle7) {
	StringSpannungsZelle7 = stringSpannungsZelle7;
}
public static String getStringSpannungsZelle9() {
	return StringSpannungsZelle9;
}
public static void setStringSpannungsZelle9(String stringSpannungsZelle9) {
	StringSpannungsZelle9 = stringSpannungsZelle9;
}
public static String getStringSpannungsZelle10() {
	return StringSpannungsZelle10;
}
public static void setStringSpannungsZelle10(String stringSpannungsZelle10) {
	StringSpannungsZelle10 = stringSpannungsZelle10;
}
public static String getStringSpannungsZelle8() {
	return StringSpannungsZelle8;
}
public static void setStringSpannungsZelle8(String stringSpannungsZelle8) {
	StringSpannungsZelle8 = stringSpannungsZelle8;
}
public static String getStringSpannungsZelle11() {
	return StringSpannungsZelle11;
}
public static void setStringSpannungsZelle11(String stringSpannungsZelle11) {
	StringSpannungsZelle11 = stringSpannungsZelle11;
}
public static String getStringSpannungsZelle13() {
	return StringSpannungsZelle13;
}
public static void setStringSpannungsZelle13(String stringSpannungsZelle13) {
	StringSpannungsZelle13 = stringSpannungsZelle13;
}
public static String getStringSpannungsZelle12() {
	return StringSpannungsZelle12;
}
public static void setStringSpannungsZelle12(String stringSpannungsZelle12) {
	StringSpannungsZelle12 = stringSpannungsZelle12;
}
public static String getStringSpannungsZelle14() {
	return StringSpannungsZelle14;
}
public static void setStringSpannungsZelle14(String stringSpannungsZelle14) {
	StringSpannungsZelle14 = stringSpannungsZelle14;
}
public static String getStringSpannungsZelle15() {
	return StringSpannungsZelle15;
}
public static void setStringSpannungsZelle15(String stringSpannungsZelle15) {
	StringSpannungsZelle15 = stringSpannungsZelle15;
}
public static int getUpdatedCellCount() {
	return UpdatedCellCount;
}
public static void setUpdatedCellCount(int updatedCellCount) {
	UpdatedCellCount = updatedCellCount;
}

    public static boolean isNew_light_status_message() {
        return New_light_status_message;
    }

    public static void setNew_light_status_message(boolean new_light_status_message) {
        MainManager.New_light_status_message = new_light_status_message;
    }

    public static boolean isNew_gesture_message() {
        return New_gesture_message;
    }

    public static void setNew_gesture_message(boolean new_gesture_message) {
        New_gesture_message = new_gesture_message;
    }

    public static boolean isNew_bike_message_totower() {
        return New_bike_message_totower;
    }

    public static void setNew_bike_message_totower(boolean new_bike_message_totower) {
        New_bike_message_totower = new_bike_message_totower;
    }

    public static boolean isNew_bike_message_fromtower() {
        return New_bike_message_fromtower;
    }

    public static void setNew_bike_message_fromtower(boolean new_bike_message_fromtower) {
        New_bike_message_fromtower = new_bike_message_fromtower;
    }
}


