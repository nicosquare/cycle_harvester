exec = require('child_process').exec;
os = require('os');
ina219 = require('./ina219.js');

const http = require('http');
const hostname = os.networkInterfaces()['apcli0'][0].address;
const port = 3000;

const server = http.createServer((req, res) => {

	if (req.url === '/?LED=ON') exec('fast-gpio set 1 1');
	if (req.url === '/?LED=OFF') exec('fast-gpio set 1 0');
	res.statusCode = 200;
	res.setHeader('Content-Type', 'text/html');
	res.write('<h2>A simple LED control server <br>with Onion Omega2+</h2>');
	res.write('<p>LED state: <a href="?LED=ON"> <button>ON</button></a>');
	res.write('  <a href="?LED=OFF"><button>OFF</button></a></p>');
	res.end();
});


  
server.listen(port, hostname, () => {

	var sensor = new ina219.sensor(ina219.INA219_ADDRESS);

	sensor.setCalibration_32V_1A();
	
	var shuntvoltage = 0;
	var busvoltage = 0;
	var current_mA = 0;
	var loadvoltage = 0;
	var power_mW = 0;
	
	sensor.getBusVoltage_raw();
	//sensor.getShuntVoltage_raw();
	//sensor.getCurrent_raw();
	//sensor.getPower_raw();
	
	//shuntvoltage = sensor.getShuntVoltage_mV();
    busvoltage = sensor.getBusVoltage_V();
    //current_mA = sensor.getCurrent_mA();
    //power_mW = sensor.getPower_mW();
    
    loadvoltage = busvoltage + (shuntvoltage / 1000);
	
	console.log("Shunt Voltage: "+shuntvoltage);
	console.log("Bus Voltage: "+busvoltage);
	console.log("Current mA: "+current_mA);
	console.log("Load Voltage: "+loadvoltage);
	console.log("Power mW: "+power_mW);
	
	console.log(`Server running at http://${hostname}:${port}/`);
});
