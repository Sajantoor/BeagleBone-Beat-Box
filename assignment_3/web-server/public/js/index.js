"use strict";
// Client-side interactions with the browser.

// Make connection to server when web page is fully loaded.
var socket;
let hasReplied = false;
let timer; // timer for checking if the program is running on the BeagleBone

function connectToSocket() {
	try {
		socket = io.connect();
	} catch (e) {
		showError("Failed to connect to server, trying again.", 1000);
		setTimeout(connectToSocket, 1000);
	}
}

function showError(message, timeout) {
	$("#error-box").show();
	$("#error-text").text(message);
	if (timeout) {
		setTimeout(() => {
			$("#error-box").hide();
		}, timeout);
	}
}

connectToSocket();

$(document).ready(() => {
	console.log("Document ready");

	window.setInterval(function () {
		sendRequest("uptime");
	}, 1000);

	$("#modeNone").click(() => {
		sendCommandViaUDP("changeMode none");
	});

	$("#modeRock").click(() => {
		sendCommandViaUDP("changeMode rock");
	});

	$("#modeCustom").click(() => {
		sendCommandViaUDP("changeMode custom");
	});

	$("#volumeDown").click(() => {
		sendCommandViaUDP("changeVolume -");
	});

	$("#volumeUp").click(() => {
		sendCommandViaUDP("changeVolume +");
	});

	$("#stop").click(() => {
		sendCommandViaUDP("shutdown");
	});

	$("#tempoDown").click(() => {
		sendCommandViaUDP("changeTempo -");
	});

	$("#tempoUp").click(() => {
		sendCommandViaUDP("changeTempo +");
	});

	$("#snare").click(() => {
		sendCommandViaUDP("playSound snare");
	});

	$("#bassDrum").click(() => {
		sendCommandViaUDP("playSound bassDrum");
	});

	$("#hihat").click(() => {
		sendCommandViaUDP("playSound hiHat");
	});

	socket.on("commandReply", function (result) {
		hasReplied = true;
		const command = result.split(" ")[0];
		const argument = result.split(" ")[1];

		if (command === "all") {
			const tokens = result.split(" ");
			console.log(tokens);
			setMode(tokens[2]);
			$("#tempoid").val(tokens[4]);
			$("#volumeid").val(tokens[6]);
		} else if (command === "volume") {
			$("#volumeid").val(argument);
		} else if (command === "tempo") {
			$("#tempoid").val(argument);
		} else if (command == "beatType") {
			setMode(argument);
		}
	});

	socket.on("fileContents", function (result) {
		var fileName = result.fileName;
		var contents = result.contents;
		let seconds = Math.floor(contents.split(" ")[0]);
		let minutes = Math.floor(seconds / 60);
		let hours = Math.floor(minutes / 60);

		hours = hours % 60;
		seconds = seconds % 60;

		if (fileName === "uptime") {
			$("#status").text(
				`Device up for: ${hours}:${minutes}:${seconds} (H:M:S)`
			);
		}
	});

	socket.on("error", function (err) {
		// show error message
		showError("There was an error in the socket connection: " + err, 5000);
	});
});

function replaceAll(str, find, replace) {
	return str.replace(new RegExp(find, "g"), replace);
}

function sendRequest(file) {
	socket.emit("proc", file);
}

function sendCommandViaUDP(message) {
	socket.emit("daUdpCommand", message);
	hasReplied = false;
	// if there's no reply within 1 second, assume the bbg is offline,
	// show an error message
	checkResponse();
}

function checkResponse() {
	// reset the timer if called again
	$("#error-box").hide();
	clearTimeout(timer);

	timer = setTimeout(() => {
		if (!hasReplied) {
			showError(
				"The program is not running on the BeagleBone. Please check the program and try again.",
				null
			);
		} else {
			$("#error-box").hide();
		}
	}, 1000);
}

function setMode(argument) {
	if (argument == 0) {
		$("#modeid").text("None");
	} else if (argument == 1) {
		$("#modeid").text("Rock");
	} else if (argument == 2) {
		$("#modeid").text("Custom");
	}
}
