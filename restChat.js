// Rest based chat client
// Jim Skon 2022
// Kenyon College

var baseUrl = 'http://54.198.38.17:5005';
var state="off";
var myname="";
var inthandle;
var inthandleUsers;
var currentUsers = [];
var masterUsers = [];

/* Start with text input and status hidden */
document.getElementById('chatinput').style.display = 'none';
document.getElementById('status').style.display = 'none';
document.getElementById('leave').style.display = 'none';

/* Set up buttons */
// Action if they push the join button
document.getElementById('login-btn').addEventListener("click", (e) => {
	join();
})

// function to register user
document.getElementById('register-btn').addEventListener("click", (e) => {
	registerUser();
})

// Action if they push the leave button
document.getElementById('leave-btn').addEventListener("click", (e) => {
	leaveSession();
})
document.getElementById('send-btn').addEventListener("click", sendText);
// Action if they push enter on message box
document.getElementById('message').addEventListener("keydown", (e)=> {
    if (e.code == "Enter") {
	e.preventDefault();
    	sendText();
    	document.getElementById('message').value = "";
    	return false;
    }   
});



// Call function on page exit
window.onbeforeunload = leaveSession;

/* Join processes */
function join() {
	myname = document.getElementById('yourname').value;
	fetch(baseUrl+'/chat/join/'+myname, {
        method: 'get'
    })
    .then (response => response.json() )
    .then (data => completeJoin(data))
    .catch(error => {
        {alert("Error: Something went wrong:"+error);}
    })
}

function completeJoin(results) {
	var user = results['user'];
	if (currentUsers.includes(user) == true){
		alert("Username already exists!");
		leaveSession();
		return;
	}
	console.log("Join:"+user);
	startSession(user);
	logUser(user);
}

/* Log users (only on client side right now) */
function logUser(user) {
	if (currentUsers.includes(user) == false) {
		currentUsers.push(user);
	};
	for (var i = 0; i < currentUsers.length; i++) {
  		document.getElementById("members").innerHTML += (i+1) + ": " + currentUsers[i] + " ";
	};
}

function removeUser(user) {
	for (var i = 0; i < currentUsers.length; i++) {
		if (user == currentUsers[i]){
			currentUsers.splice(i,1);
		}
	};
	document.getElementById("members").innerHTML = " ";
	for (var i = 0; i < currentUsers.length; i++) {
  		document.getElementById("members").innerHTML += (i+1) + ": " + currentUsers[i] + " ";
	};
}

/* Register Users */
function registerUser() {
	username = document.getElementById('register-name');
	email = document.getElementById('register-email');
	password = document.getElementById('register-password');
	fetch(baseUrl+'/chat/join/'+username+'/'+email+'/'+password, {
		method: 'get'
	})
	.then (response => response.json())
	.then (data => completeRegisterUser(data))
	.catch(error => {
		{alert("Error: Something went wrong:"+error);}
	})
}

function completeRegisterUser(results) {
	var status = results['status'];
	if (status != "success") {
		alert("Username or email already exists!");
		leaveSession();
		return;
	}
	var user = results['user'];
	console.log("Register:"+user);
	startSession(user);
	logUsers(user);
}

function completeSend(results) {
	var status = results['status'];
	if (status == "success") {
		console.log("Send succeeded")
	} else {
		alert("Error sending message!");
	}
}

//function called on submit or enter on text input
function sendText() {
    var message = document.getElementById('message').value;
    console.log("Send: "+myname+":"+message);
	fetch(baseUrl+'/chat/send/'+myname+'/'+message, {
        method: 'get'
    })
    .then (response => response.json() )
    .then (data => completeSend(data))
    .catch(error => {
        {alert("Error: Something went wrong:"+error);}
    })    
	document.getElementById('message').value = "";
}

function completeFetch(result) {
	// Messages
	messages = result["messages"];
	messages.forEach(function (m,i) {
		name = m['user'];
		message = m['message'];
		document.getElementById('chatBox').innerHTML +=
	    	"<font color='red'>" + name + ": </font>" + message + "<br />";
	});
	// Users
	users = result["userList"];
	users.forEach(function (m,i) {
		name = m['user'] + ", ";
		if (masterUsers.includes(name) == false) {
			masterUsers.push(name);
		}
	});
}

/* Check for new messaged */
function fetchMessage() {
	fetch(baseUrl+'/chat/fetch/'+myname, {
        method: 'get'
    })
    .then (response => response.json() )
    .then (data =>completeFetch(data))
    .catch(error => {
        {console.log("Server appears down");}
    })  	
}

/* Functions to set up visibility of sections of the display */
function startSession(name){
    state="on";
    document.getElementById('yourname').value = "";
    document.getElementById('register').style.display = 'none';
    document.getElementById('user').innerHTML = "User: " + name;
    document.getElementById('chatinput').style.display = 'block';
    document.getElementById('status').style.display = 'block';
    document.getElementById('leave').style.display = 'block';
    /* Check for messages every 500 ms */
    inthandle=setInterval(fetchMessage,500);
    /* Check for current users every 500 ms */
    //inthandleUsers=setInterval(updateUsers,500);
}

function leaveSession(){
    state="off";
    logout();
    document.getElementById('yourname').value = "";
    document.getElementById('register').style.display = 'block';
    document.getElementById('user').innerHTML = "";
    document.getElementById('chatinput').style.display = 'none';
    document.getElementById('members').style.display = 'none';
    document.getElementById('status').style.display = 'none';
    document.getElementById('leave').style.display = 'none';
	clearInterval(inthandle);
	//clearInterval(inthandleUsers);
}

function logout() {
	fetch(baseUrl+'/chat/logout/'+myname, {
		method: 'get'
	})
	.then (response => response.json() )
	.then (data => completeLogout(data) )
	.catch(error => {
		{console.log("Service Unavailable");}
	})
}

function completeLogout(user) {
	removeUser(user);
}

// I wrote code for registration

// Get the register form and submit button
const registerForm = document.querySelector('#loginModal form');
const submitButton = document.querySelector('#loginModal .modal-footer button.btn-primary');

// Add event listener to the submit button
submitButton.addEventListener('click', function(event) {
  event.preventDefault(); // prevent form from submitting normally
  
  // Get the input values
  const name = registerForm.querySelector('#orangeForm-name').value;
  const email = registerForm.querySelector('#orangeForm-email').value;
  const password = registerForm.querySelector('#orangeForm-pass').value;
  
  // Do some basic client-side validation
  if (!name || !email || !password) {
    alert('Please fill in all fields');
    return;
  }
  
  // Send the registration data to the server
  const xhr = new XMLHttpRequest();
  xhr.open('POST', '/register');
  xhr.setRequestHeader('Content-Type', 'application/json');
  xhr.onload = function() {
    if (xhr.status === 200) {
      alert('Registration successful!');
      // Optionally, you can redirect the user to a new page here
    } else {
      alert('Registration failed');
    }
  };
  xhr.send(JSON.stringify({ name, email, password }));
  
  // Close the modal
  const modal = bootstrap.Modal.getInstance(document.querySelector('#loginModal'));
  modal.hide();
  
  // Clear the form inputs
  registerForm.reset();
});

