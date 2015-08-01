import thread
import time
import serial
import urllib2
import json
from xml.dom.minidom import parseString

refreshRate = 600 #in seconds (10 minutes)
strPreviousWeather = ''
strPreviousNews = ''
Quit = 'Q'

#start serial
try:
    #try to establish serial connection and delay 5 seconds
    #in order to create stability (the arduino will require
    #some time in order to get started). Establishing connections
    #can sometimes take a while, especially when serving
    #from a raspberry pi

    print "establishing serial connection"
    ser = serial.Serial("\\.\COM3", 9600)
    time.sleep(5)
except:
    #if connection fails, sleep for 10 seconds and retry...
    print "unable to connect, waiting 10 seconds"
    time.sleep(10)
    try:
        #try again and if failed print a final message
        print "attempting to establish connection again"
        ser = serial.Serial("\\.\COM3", 9600)
        time.sleep(5)
    except:
        print "no connection available, please check the port configuration"

#--------------WEATHER--------------------------------------------------------

def sendWeather():
        #access global variable
        global strPreviousWeather
        
        #get location data
        print "locating"
        geourl = "http://ip-api.com/json"

        try:
            response = urllib2.urlopen(geourl)   
        except:
            print "unable to establish connection, retrying in 10s"
            time.sleep(10)
            try:    
                response = urllib2.urlopen(geourl)    
            except:
                print "connection not available, please check your internet connection"
                ser.write('{')
                return None
                #exit to avoid errors, can be fixed with a boolean hasData value and if 
                #statement then if check fails, send a default message.
        
        response = urllib2.urlopen(geourl)
        content = response.read()
        geodata = json.loads(content.decode("utf-8"))

        #get weather data based on location
        apikey = "&APPID=912a9af3412ddaa3903bfc538c1056bd"
        print "gathering weather data..."
        weatherurl = "http://api.openweathermap.org/data/2.5/weather?q=" + str(geodata['city']) + ',' + str(geodata['countryCode']) + "&units=metric" + apikey
        wresponse = urllib2.urlopen(weatherurl)
        wcontent = wresponse.read()
        wdata = json.loads(wcontent.decode("utf-8"))

        #parse the data for formatting - formatting done this way in order to
        #have more flexibility with the presentation of the string
        print "parsing weather data"
        cityname = geodata['city'] + ", "
        weather = wdata['weather'][0]['main']
        temp = wdata['main']['temp']
        temp = '%.1f'%temp + chr(223) + "C"
        country = geodata['country']

        #create proper length strings with spaces between values
        iterate = 16 -(len(cityname) +len(weather))
        strspaces = ""
        j=0
        while j < iterate:
            strspaces += " "
            j +=1

        iterate2 = 16 - (len(country) + len(temp))

        #same for second line
        strspaces2 = ""
        k=0
        while k < iterate2:
             strspaces2 += " "
             k +=1

        #create a string (!!has to be UNDER 64 char long - buffer limitations) //add check eventually
        strWeather = str(cityname) + strspaces + str(weather) + str(country) + strspaces2 + temp

        #check for updates, if new data is available write validation token and send new weather data
        if strWeather != strPreviousWeather:
            print "sending weather data..."
            strPreviousWeather = strWeather
            ser.write('^')
            ser.write(strWeather)
        else:
            #send weather confirmation token
            print "no new weather data available"
            ser.write('{')
            
        
#--------------NEWS-----------------------------------------------------------
def sendNews():
        #access global variable
        global strPreviousNews
        
        #Get the news data -- Code can work with any xml, but adaptations
        #will have to be made
        print "gathering news data..."

        newsurl = 'http://feeds.bbci.co.uk/news/rss.xml'
        try:
            file = urllib2.urlopen(newsurl)   
        except:
            print "unable to establish connection, retrying in 10s"
            time.sleep(10)
            try:    
                file = urllib2.urlopen(newsurl)    
            except:
                print "connection not available, please check your internet connection"
                ser.write('}')
                return None
                #exit to avoid errors, can be fixed with a boolean hasData value and if 
                #statement then if check fails, send a default message.

        #file = urllib2.urlopen('http://news.sky.com/feeds/rss/world.xml')
        data = file.read()
        file.close()

        #parse the data
        print "parsing news data"
        dataToSend = 0
        dom = parseString(data)
        xmlTag = dom.getElementsByTagName('title')[2].toxml()

        #trim the tags and convert to string(!!note that this too has to be UNDER 64 char long)
        print "formatting news data" 
        xmlData=xmlTag.replace('<title>','').replace('</title>','')
        strNews = str(xmlData)
        
        #convert to string, send a validation token and send data
        if strNews != strPreviousNews:    
            print "sending news data..."
            strPreviousNews = strNews
            ser.write('~')
            ser.write(strNews)
        else:
            #send news confirmation token
            print "No new news data available"
            ser.write('}')

try:
    while 1:
        #to avoid connection losses or at times errors, close the connection
        #ser.close()
        
        #sleep for desired refresh amount and restart the while loop
        sendWeather()

        #let the buffer catch up before going further (!!Might require tweaking
        #depending on the size of your string)
        time.sleep(.4)
        sendNews()
        time.sleep(refreshRate)
except KeyboardInterrupt:
    print "^C ---- closing server ---- "
    ser.close()
    import sys
    sys.exit(0)


