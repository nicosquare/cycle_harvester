exec = require('child_process').exec;
os = require('os');
ina219 = require('./ina219.js');

const http = require('http');
const hostname = os.networkInterfaces()['apcli0'][0].address;
const port = 3000;

const sensor = new ina219.sensor(ina219.INA219_ADDRESS);

const server = http.createServer((req, res) => {

	if (req.url === '/?INFO=GET'){
		
		var shuntvoltage = 0;
		var busvoltage = 0;
		var current_mA = 0;
		var loadvoltage = 0;
		var power_mW = 0;
	
		shuntvoltage = sensor.getShuntVoltage_mV();
		busvoltage = sensor.getBusVoltage_V();
		current_mA = sensor.getCurrent_mA();
		power_mW = sensor.getPower_mW();
		
		loadvoltage = busvoltage + (shuntvoltage / 1000);
		
		console.log("Shunt Voltage: "+shuntvoltage);
		console.log("Bus Voltage: "+busvoltage);
		console.log("Current mA: "+current_mA);
		console.log("Load Voltage: "+loadvoltage);
		console.log("Power mW: "+power_mW);
		
	}
	
	if (req.url === '/?INFO=UPDATE'){
		
		console.log("Electric measures updated");
			
		sensor.getBusVoltage_raw(sensor.updateBusVoltage_raw.bind(sensor));
		sensor.getShuntVoltage_raw(sensor.updateShuntVoltage_raw.bind(sensor));
		sensor.getCurrent_raw(sensor.updateCurrent_raw.bind(sensor));
		sensor.getPower_raw(sensor.updatePower_raw.bind(sensor));
	
	}
	
	res.statusCode = 200;
	res.setHeader('Content-Type', 'text/html');
	res.write('<h2> A simple test program for Cycle Harvester<br>with Onion Omega2+</h2>');
	res.write('<p>Get electric measures: <a href="?INFO=GET"> <button>Get</button></a>');
	res.write('  <a href="?INFO=UPDATE"><button>Update</button></a></p>');
	res.end();
});


  
server.listen(port, hostname, () => {

	sensor.setCalibration_32V_1A();
	
	sensor.getBusVoltage_raw(sensor.updateBusVoltage_raw.bind(sensor));
	sensor.getShuntVoltage_raw(sensor.updateShuntVoltage_raw.bind(sensor));
	sensor.getCurrent_raw(sensor.updateCurrent_raw.bind(sensor));
	sensor.getPower_raw(sensor.updatePower_raw.bind(sensor));
	
	console.log(`Server running at http://${hostname}:${port}/`);
});
