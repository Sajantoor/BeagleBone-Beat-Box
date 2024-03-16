"use strict";
/*
 * Respond to commands over a websocket to relay UDP commands to a local program
 */

var socketio = require('socket.io');
var io;

var dgram = require('dgram');
var fs = require('fs');

const UDP_SERVER_ADDRESS = "127.0.0.1";
const UDP_SERVER_PORT = 7070;

const UDP_BBG_ADDRESS = "192.168.6.2";
const UDP_BBG_PORT = 12345;

exports.listen = function (server) {
	io = socketio.listen(server);
	io.set('log level 1');

	var udp_server = dgram.createSocket('udp4');
	udp_server.bind(UDP_SERVER_PORT, UDP_SERVER_ADDRESS);

	io.sockets.on('connection', function (socket) {
		handleConnection(socket, udp_server);
	});

};

function handleConnection(socket, udp_server) {
	// WebSocket command gets send as a UDP message to the local process.
	socket.on('daUdpCommand', function (data) {
		console.log('daUdpCommand command: ' + data);

		// Info for connecting to the local process via UDP
		var client = dgram.createSocket('udp4');
		var buffer = new Buffer(data);

		client.send(buffer, 0, buffer.length, UDP_BBG_PORT, UDP_BBG_ADDRESS, function (err, bytes) {
			if (err)
				throw err;
			console.log('UDP message sent to ' + UDP_BBG_PORT + ':' + UDP_BBG_ADDRESS);

			client.close();
		});
	});

	socket.on('proc', function (fileName) {
		// NOTE: Very unsafe? Why?
		// Hint: think of ../
		var absPath = "/proc/" + fileName;
		console.log('accessing ' + absPath);

		fs.exists(absPath, function (exists) {
			if (exists) {
				// Can use 2nd param: 'utf8', 
				fs.readFile(absPath, function (err, fileData) {
					if (err) {
						emitSocketData(socket, fileName,
							"ERROR: Unable to read file " + absPath);
					} else {
						emitSocketData(socket, fileName,
							fileData.toString('utf8'));
					}
				});
			} else {
				emitSocketData(socket, fileName,
					"ERROR: File " + absPath + " not found.");
			}
		});
	});

	udp_server.on('listening', function () {
		var address = udp_server.address();
		console.log('UDP Client: listening on ' + address.address + ":" + address.port);
	});

	// Handle an incoming message over the UDP from the local application.
	udp_server.on('message', function (message, remote) {
		console.log("UDP Client: message Rx" + remote.address + ':' + remote.port + ' - ' + message);

		var reply = message.toString('utf8')
		socket.emit('commandReply', reply);
	});

	udp_server.on("UDP Client: close", function () {
		console.log("closed");
		udp_server.close();
	});

	udp_server.on("UDP Client: error", function (err) {
		console.log("error: ", err);
		udp_server.close();
	});
};

function emitSocketData(socket, fileName, contents) {
	var result = {
		fileName: fileName,
		contents: contents
	}
	socket.emit('fileContents', result);
}


