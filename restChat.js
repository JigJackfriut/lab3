// Rest based chat client
// Jim Skon 2022
// Kenyon College

var baseUrl = 'http://54.198.38.17:5005';
var state="off";
var myname="";
var mypass="";
var myemail="";
var inthandle;
var inthandleUsers;
var currentUsers = [];
var masterUsers = [];

/* Start with text input and status hidden */
document.getElementById('chatinput').style.display = 'none';
document.getElementById('status').style.display = 'none';
document.getElementById('leave').style.display = 'none';
document.getElementById('disap').style.display = 'none';
document.getElementById('invitebutton').style.display = 'none';


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
  .then (data => {
    // If the button is checked, call completeFetchDispa(), otherwise call completeFetch()
    if (document.getElementById('disappearButton').checked) {
      completeFetchDi(data);
    } else {
      completeFetch(data);
    }
  })
  .catch(error => {
    console.log("Server appears down");
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
    document.getElementById('members').innerHTML = name;
    document.getElementById('disap').style.display = 'block';
    document.getElementById('invitebutton').style.display = 'block';
    /* Check for messages every 500 ms */
    inthandle=setInterval(fetchMessage,500);
    /* Check for current users every 500 ms */
    inthandleUsers=setInterval(allUsers,500);
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

// Define a function to handle registration form submission


function registerUser() { 
  // Get the form input values
  const name = document.getElementById("orangeForm-name").value;
  const email = document.getElementById("orangeForm-email").value;
  const password = document.getElementById("orangeForm-pass").value;

  // Validate the input
  if (!name || !email || !password) {
    alert("Please fill out all fields");
    return;
  } else if (password.length < 6) {
    alert("Password must be at least 6 characters long");
    return;
  } else {
    myname = name;
    myemail = email;
    mypass = password;

    // Send user credentials to server for registration
    fetch(baseUrl+'/chat/register/'+myname+'/'+myemail+'/'+mypass, {
      method: 'get'
    })
    .then(response => response.json())
    .then(data => {
      if (data.error) {
        // Registration failed due to duplicate username
        alert(data.error);
      } else {
        // Registration successful
        alert(data.message);
        startSession(myname);
      }
    })
    .catch(error => {
      console.log("Server appears down");
    });
  }
}




// Attach the registerUser function to the form submit button
const submitButton = document.getElementById("saveChangesButton");
submitButton.addEventListener("click", registerUser);

// login stuff



function loginUser() { 
  // Get the form input values
  const name = document.getElementById("yourname").value;
  const password = document.getElementById("yourpass").value;

  // Validate the input
  if (!name || !password) {
    alert("Please fill out all fields");
    return;
  } else if (password.length < 6) {
    alert("Password must be at least 6 characters long");
    return;
  } else {
    myname = name;
    mypass = password;

    // Send user credentials to server for registration
    fetch(baseUrl+'/chat/login/'+myname+'/'+mypass, {
      method: 'get'
    })
    .then(response => response.json())
    .then(data => {
      if (data.error) {
         alert(data.error);
	     return;
      } else {
        // Registration successful
        alert(data.message);
        startSession(myname);
      }
    })
    .catch(error => {
      console.log("Server appears down");
    });
  }
}

const loginButton = document.getElementById("login-btn");
loginButton.addEventListener("click", loginUser);









function allUsers() {
	fetch(baseUrl+'/chat/list', {
        method: 'get'
    })
    .then (response => response.json() )
    .then (data =>updateUsers(data))
    .catch(error => {
        {alert("Error: Something went wrong:"+error);}
    })
}
function updateUsers(result) {
	userList = result["userList"];
	//console.log("user list printed");
	document.getElementById('members').innerHTML = userList;
}




function completeFetchDi(result) {
  // Messages
  messages = result["messages"];
  messages.forEach(function(m, i) {
    name = m['user'];
    message = m['message'];
    var messageElement = document.createElement('div');
    messageElement.classList.add('message');
    messageElement.innerHTML = "<font color='red'>" + name + ": </font>" + message;
    document.getElementById('chatBox').appendChild(messageElement);

    // Set a timeout to remove the message after 30 seconds
    setTimeout(function() {
      messageElement.remove();
    }, 5000);
  });

  // Users
  users = result["userList"];
  users.forEach(function(m, i) {
    name = m['user'] + ", ";
    if (masterUsers.includes(name) == false) {
      masterUsers.push(name);
    }
  });
}
 
