"use strict";
// Client-side interactions with the browser.

// Timeout flag for connection to application
var requestTimeout = false;

// Make connection to server when web page is fully loaded.
var socket = io.connect();
$(document).ready(function() {

	window.setInterval(function() {sendCommandViaUDP('uptime')}, 1000)
	window.setInterval(function() {sendCommandViaUDP('tempo')}, 1000)
	window.setInterval(function() {sendCommandViaUDP('volume')}, 1000)
	window.setInterval(function() {sendCommandViaUDP('mode')}, 1000)

	$('#modeNone').click(function(){
		sendCommandViaUDP("mode 0");
	});
	$('#modeRock1').click(function(){
		sendCommandViaUDP("mode 1");
	});
	$('#modeRock2').click(function(){
		sendCommandViaUDP("mode 2");
	});
	$('#volumeDown').click(function(){
		sendCommandViaUDP("voldown");
	});
	$('#volumeUp').click(function(){
		sendCommandViaUDP("volup");
	});
	$('#tempoDown').click(function(){
		sendCommandViaUDP("tempodown");
	});
	$('#tempoUp').click(function(){
		sendCommandViaUDP("tempoup");
	});
	$('#playHiHat').click(function(){
		sendCommandViaUDP("playhihat");
	});
	$('#playSnare').click(function(){
		sendCommandViaUDP("playsnare");
	});
	$('#playBase').click(function(){
		sendCommandViaUDP("playbase");
	});
	$('#stop').click(function(){
		sendCommandViaUDP("terminate");
	});

	socket.on('disconnect', () => {
		$('#error-box').show();
		$('#error-text').html("NodeJS server is no longer running on the target.");
	})

	socket.on('reconnect', () => {
		$('#error-box').hide();
	})
	
	socket.on('commandReply', function(result) {
		var splitRes = result.split(",");
		// clearTimeout(requestTimeout);

		var domObj;
		switch(splitRes[0]) {
		case 'modeid':
			$('#error-box').hide();
			domObj = $('#modeid');
			domObj.html(splitRes[1]);
			break;
		case 'uptime':
			domObj = $('#status');
			var h = splitRes[1];
			var m = splitRes[2];
			var s = splitRes[3];
			domObj.html("Device up for: " + h + ":" + m + ":" + s + "(H:M:S)");
			break;
		case 'volumeid':
			domObj = $('#volumeid');
			domObj.val(splitRes[1]);
			break;
		case 'tempoid':
			domObj = $('#tempoid');
			domObj.val(splitRes[1]);
			break;
		case 'noreply':
			$('#error-box').show();
			$('#error-text').html("Beatbox C application is no longer running.");
			break;
		case 'soundreq':
		case 'terminate':
			break;
		default:
			console.log("Unknown DOM object: " + splitRes[0]);
			return;
		}
	});
	
});

function sendCommandViaUDP(message) {
	socket.emit('daUdpCommand', message);

	// clearTimeout(requestTimeout);
	// requestTimeout = setTimeout(function() {
	// 	$('#error-box').show();
	// }, 3000)
};