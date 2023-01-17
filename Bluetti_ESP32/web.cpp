#include <WiFi.h>

#include "web.h"
#include "DataStore.h"
#include "DeviceType.h"
#include "MQTT.h"
#include "BTooth.h"
#include "BluettiConfig.h"
#include "utils.h"


void setState(enum field_names field_name, int enabled) {
    bt_command_t command;
    command.prefix = 0x01;
    command.field_update_cmd = 0x06;

    for (int i=0; i< sizeof(bluetti_device_command)/sizeof(device_field_data_t); i++) {
        if (field_name == bluetti_device_command[i].f_name) {
            command.page = bluetti_device_command[i].f_page;
            command.offset = bluetti_device_command[i].f_offset;
        }
    }

    command.len = swap_bytes(enabled);
    command.check_sum = modbus_crc((uint8_t*)&command,6);

    sendBTCommand(command);
}

const char* mainsite PROGMEM = R"""(
<!DOCTYPE html>
<html>
    <head>
        <meta name="viewport" content="width=device-width,initial-scale=1,user-scalable=no"/>
        <title>BLUETTI Web View</title>
        <h1>BLUETTI Web View</h1>
        <style> .c{text-align:center;}div,input{padding:5px;font-size:1em;}input{width:95%;}body{text-align:    center;font-family:verdana;background-color:#283747;color:#fff;}button{border:0;border-radius:0.3rem;background-color:#1fa3ec;color:#fff;line-height:2.4rem;font-size:1.2rem;width:100%;} .q{float:right;width:64px;text-align:right;} 
        </style>
    </head>
    <body>
        <div style=\"text-align:left;display:inline-block;min-width:260px;\">
            <p id="table"></p>
            <br>
            <br>
            <form action="/ac_set_state" method="post" target="dummyframe">
                <button name="state" value="1">AC ON</button>
            </form>
            <br>
            <form action="/ac_set_state" method="post" target="dummyframe">
                <button name="state" value="0">AC OFF</button>
            </form>
            <br>
            <br>
            <form action="/dc_set_state" method="post" target="dummyframe">
                <button name="state" value="1">DC ON</button>
            </form>
            <br>
            <form action="/dc_set_state" method="post" target="dummyframe">
                <button name="state" value="0">DC OFF</button>
            </form>
        </div>
        <iframe name="dummyframe" id="dummyframe" style="display: none;"></iframe>
        <script>
            function writeTable() {
                var xmlHttp = new XMLHttpRequest();
                xmlHttp.onreadystatechange = function() {
                    if (xmlHttp.status == 200 && xmlHttp.responseText != "") {
                        tbl = "<table>";

                        data = JSON.parse(xmlHttp.responseText);
                        const ordered_data = Object.keys(data).sort().reduce(
                            (obj, key) => {
                                obj[key] = data[key];
                                return obj;
                            },
                            {}
                        );
                        Object.entries(ordered_data).forEach((entry) => {
                            const [key, value] = entry;
                            tbl += "<tr><td>" + key + "</td><td>" + value + "</td></tr>";
                        });
                        tbl += "</table>";
                        document.getElementById("table").innerHTML = tbl;
                    }
                }
                xmlHttp.open("GET", "values", true);
                xmlHttp.send(null);
            }
            writeTable();
            setInterval(writeTable, 1000);
        </script>
    </body>
</html>
)""";

void initWeb(WebServer& httpServer) {

    httpServer.on("/panel", HTTP_GET, [&httpServer]() {
    	httpServer.send(200, F("text/html"), mainsite);
    });

    httpServer.on("/values", HTTP_GET, [&httpServer]() {
        bool first = true;
        String s = "{ ";
        for (int i = 0; i < FIELD_NAMES_LAST; ++i) {
            String value = ds.getValueAsJson((enum field_names)i);
            if (!value.isEmpty()) {
                if (!first) {
                    s += ",";
                } else {
                    first = false;
                }
                s += "\"" + map_field_name((enum field_names)i) + "\":\"" + value + "\"";
            }
        }
        s += " }";
    	httpServer.send(200, F("application/json"), s);
    });
    
    httpServer.on("/ac_set_state", HTTP_POST, [&httpServer]() {
        String v = httpServer.arg("state");
        setState(AC_OUTPUT_ON, v == "1");
        Serial.println(String("Set AC state to ") + (v == "1" ? "ON" : "OFF"));
     	httpServer.send(200, F("text/html"), F("OK"));
    });

    httpServer.on("/dc_set_state", HTTP_POST, [&httpServer]() {
        String v = httpServer.arg("state");
        setState(DC_OUTPUT_ON, v == "1");
        Serial.println(String("Set DC state to ") + (v == "1" ? "ON" : "OFF"));
        httpServer.send(200, F("text/html"), F("OK"));
    });
}
