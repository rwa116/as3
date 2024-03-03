"use strict";
// Client-side interactions with the browser.

// Make connection to server when web page is fully loaded.
var socket = io.connect();
$(document).ready(function() {

	//window.setInterval(function() {sendRequest('modeid')}, 1000)

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
	
	socket.on('commandReply', function(result) {
		var splitRes = result.split(",");

		var domObj;
		switch(splitRes[0]) {
		case 'modeid':
			domObj = $('#modeid');
			domObj.html(splitRes[1]);
			break;
		case 'volumeid':
			domObj = $('#volumeid');
			domObj.val(splitRes[1]);
			break;
		case 'tempoid':
			domObj = $('#tempoid');
			domObj.val(splitRes[1]);
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
};