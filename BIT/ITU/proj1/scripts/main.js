// global array with instructions. Each object contains an id of menu item and array of instructions for this menu item ({id=5, inst=["finid this", "find that"]})
var instructions = new Array();

var menuAccessKeys = "";
var menu = "";
var menuWithChck = "";
var currentInstructionId;
var instructionCount;
var maxTasks = 45;
var menuCount = 0;
var maxMenus = 3;

var menus = [];
var resultTimes = [];
var instructionsOrder = [];
var resultWrongClicks = [];
var testerAge;
var testerSex;

var testingData = {};

var actTimer = 0;
var startMilisec = 0;
var cntCorrect = 0;
var cntWrong = 0;


// This function enable to stack .css with delay
$.fn.extend({
   qcss: function(css) {
      return $(this).queue(function(next) {
         $(this).css(css);
         next();
      });
   }
});

function getMenuName(menu){
    if (menu == "menu/compact/index.html") { return "Klávesové zkratky";}
    if (menu == "menu/dropdown/index.html") { return "Horizontální menu"; }
    if (menu == "menu/vertical/index.html") { return "Vertikální menu"; }
    return "default";
}


$(document).ready(function() { 
	// modal pop-up for test beginning
	$('.modal').modal({
		// when opening modal, disable the accesskey attributes of menu items
		ready: function() { showMenu(); },
		// restore accesskey attributes
		complete: function() { showMenuAccessKeys(); }
	});

    $(".btn-preventdef").click(function(event){
        event.preventDefault();
    });
});

var submenuCnt = 0;

// converts the json with menu items and instructions into a html menu.
function buildList(data, isSub){
    var html = '';
    html += '<ul class="left hide-on-med-and-down">';

    for (item in data) {
        html += '<li>';
        if (typeof (data[item].Items) === 'object') { // An array will return 'object'
            submenuCnt++;
           
            if (isSub) {
                html += '<a href="#">' + data[item].Name + '</a>';
            } else {
                html += '<a href="#" id=\"' + data[item].Id + '\" accesskey=\"' + data[item].Key + '\">' + data[item].Name.replace(new RegExp(data[item].Key, "i"), function myFunction(x) { return '<u>' + x + '</u>'; }) + '</a>'; // Submenu found, but top level list item.
                //instructions.push({ id: data[item].Id, tasks: data[item].Inst });
            }
            html += buildList(data[item].Items, true); // Submenu found. Calling recursively same method (and wrapping it in a div)
        } else {
            html += '<a href="#" onClick="itemClick(' + data[item].Id + ')" id=\"' + data[item].Id + '\" accesskey=\"' + data[item].Key + '\">' + data[item].Name.replace(new RegExp(data[item].Key, "i"), function myFunction(x) { return '<u>' + x + '</u>'; }) + '</a>'; // No submenu
            instructions.push({ id: data[item].Id, tasks: data[item].Inst });
        }
        html += '</li>';
    }
    html += '</ul>';
    return html;
}

// converts the json with menu items and instructions into a html menu.
function buildListWithCheckbox(data, isSub) {
    var html = '';
    if (isSub) html += '<input style="display:none" type="checkbox" id="drop-' + submenuCnt + '">';
    html += '<ul class="left hide-on-med-and-down">';

    for (item in data) {
        html += '<li>';
        if (typeof (data[item].Items) === 'object') { // An array will return 'object'
            submenuCnt++;
            html += '<label style="display:none" for="drop-' + submenuCnt + '" class="toggle">' + data[item].Name + '+</label>';
            if (isSub) {
                html += '<a href="#">' + data[item].Name + '</a>';
            } else {
                html += '<a href="#" id=\"' + data[item].Id + '\" accesskey=\"' + data[item].Key + '\">' + data[item].Name.replace(new RegExp(data[item].Key, "i"), function myFunction(x) { return '<u>' + x + '</u>'; }) + '</a>'; // Submenu found, but top level list item.
                instructions.push({ id: data[item].Id, tasks: data[item].Inst });
            }
            html += buildListWithCheckbox(data[item].Items, true); // Submenu found. Calling recursively same method (and wrapping it in a div)
        } else {
            html += '<a href="#" onClick="itemClick(' + data[item].Id + ')" id=\"' + data[item].Id + '\" accesskey=\"' + data[item].Key + '\">' + data[item].Name.replace(new RegExp(data[item].Key, "i"), function myFunction(x) { return '<u>' + x + '</u>'; }) + '</a>'; // No submenu
            html += '<span class="key">Alt+' + data[item].Key.toUpperCase()+'</span>';
            instructions.push({ id: data[item].Id, tasks: data[item].Inst });
        }
        html += '</li>';
    }
    html += '</ul>';
    return html;
}

// get instruction from instruction array
var getInstruction = function(id) {
    //console.log(instructions);
    for (var i = 0, len = instructions.length; i < len; i++) {
        if (instructions[i].id === id)
            return instructions[i]; // Return as soon as the object is found
    }
    return null; // The object was not found
}

function correctFeedback(){
	if ( !$("#status").hasClass("correct") ){
		$("#status").toggleClass("wrong").toggleClass("correct").html("Správně");
	}
	$('#status').qcss( {opacity: 0.5} ).delay(500).qcss({opacity: 0});
}

function wrongFeedback(){
	if ( !$("#status").hasClass("wrong") ){
		$("#status").toggleClass("correct").toggleClass("wrong").html("Špatně");
	}
	$('#status').qcss( {opacity: 0.5} ).delay(500).qcss({opacity: 0});
}


// function that detects click on some menu item and alerts its id
function itemClick(id){
	if (currentInstructionId == id){
	    // Spravný klik
	    resultWrongClicks.push(getWrong());
	    resetCounters();
	    stopTimer();
		addCorrect();
		getRandomInstruction();
		correctFeedback();

		console.log("Correct click");
	}else{
		console.log("bad click");
		addWrong();
		wrongFeedback();
	}
}

// restores accesskey attributes of menu items
function showMenuAccessKeys(){
	$("#menu").html(menuAccessKeys);
	ak.each(function() {
		$(this).attr('accesskey', $(this).data('ak'))
	})
	
}

// disables accesskey attributes of menu items
function showMenu(){
	$("#menu").html(menu);
	ak = $('[accesskey]').each(function() {
		$(this).data('ak', $(this).attr('accesskey')).removeAttr('accesskey')
	})
		
}

//start timer when test starts
function startTimer() {	
	// gets the information from the form
	var age = $('#age').val();
	var gender = $('input[name=gender]:checked', '#personInfo').val();

	var d = new Date();
	startMilisec = d.getTime();
} 

//call to get timer time
function stopTimer() {
    var d = new Date();
    var n = d.getTime();
    var timeMilisec = n - startMilisec;

    resultTimes.push(timeMilisec);

    return timeMilisec;
} 

function addCorrect() {cntCorrect++;}  
function addWrong() {cntWrong++;}  
function resetCounters() {cntWrong = 0; cntCorrect = 0;}  
function getCorrect() {return cntCorrect;}
function getWrong() { return cntWrong; }


function shuffleMenus() {
    var i = maxMenus, randomIndex, temp;

    menus.push("menu/dropdown/index.html");
    menus.push("menu/vertical/index.html");
    menus.push("menu/compact/index.html");

    //while there are elements to shuffle
    while (0 !== i) {
        //pick a remaining element
        randomIndex = Math.floor(Math.random() * i);
        i -= 1;
        //swap it with the actual element
        temp = menus[i];
        menus[i] = menus[randomIndex];
        menus[randomIndex] = temp;
    }
}

function showResults() {
    saveResultsToJson();
    $("#main").load("pages/testComplete.html");
    $("#menu").hide();

    //must be made in testComplete page, I dont know why its not working here
    //$("h5[id^='r1']").append((resultTimes[0] + " ms"));
    //$("h5[id^='r2']").append(resultTimes[maxTasks / maxMenus] + " ms");
    //$("h5[id^='r3']").append(resultTimes[(maxTasks / maxMenus) * 2] + " ms");
}

// Získá náhodně další instrukci a vypíše ji v požadovaném formátu, pokud už je splněný počet tasků, načte se finální form
function getRandomInstruction(){
    instructionCount++;

	if( instructionCount > maxTasks){
	    showResults();
		return;
	} 

    // Get instruction by random id
	var instruction = getInstruction(Math.floor(Math.random() * instructions.length));

    // if instruction was not found, try to to get another one
    // if tasks are undefined, find another one also.
    while ( (instruction == null) || (typeof instruction.tasks === "undefined") ){ // kdyz je zamestnanes, nebo manazer, vyber jinou (tj no taks)
        instruction = getInstruction(Math.floor(Math.random() * instructions.length));
    }

    console.log(instruction);
	var html = '<div class="task">\
	<div class="progress"><div class="determinate" style="width:'+ ( ( (instructionCount-1)/maxTasks) * 100 ) +'%"></div></div>\
	<div id="currentTask" class="flow-text">'+ instruction.tasks[Math.floor(Math.random() * instruction.tasks.length)]+'</div> \
	</div>'
	$("#main").html(html);
	currentInstructionId = instruction.id;
	instructionsOrder.push(currentInstructionId);

	//alert("instrucCount:" + (instructionCount-1) + " menuCount:" + menuCount);
	if ((instructionCount-1) % (maxTasks / 3) == 0) {
	    getRandomMenu();
	}

	startTimer();
}

function saveResultsToJson(){
    testingData.menus = menus;
    testingData.wrongClicks = resultWrongClicks;
    testingData.resultTimes = resultTimes;
    testingData.instructionsOrder = instructionsOrder;
    testingData.testerAge = testerAge;
    testingData.testerSex = testerSex;

    //console.log(testingData);
    //console.log(JSON.stringify(testingData));
    //console.log(window.location.href.split("#")[0]+'save_json.php');

    //alert(window.location.href+'functions.php');
    $.ajax
    ({
        type: "POST",
        url: window.location.href.split("#")[0]+'save_json.php',
        data: { data: JSON.stringify(testingData)},
        success: function () {console.log(window.location.href.split("#")[0]+'save_json.php'); },
        failure: function() {alert("Nepovdlo se uložit, prosím, kontaktujte support.");}
    });

    // If I decide, that I want it proper way in object
    //testingData.tasks = [];
    // for (var i = 0, len = times.length; i < len; i++){
    //     var task = {};
    //     task.wrongclicks = tclicks[i];
    //     task.time = times[i];
    //     task.instruction = instructions[i];
    //     testingData.tasks.push(task);
    // }

}

function getRandomMenu() {
    $("#menu").load(menus[menuCount]);
    menuCount++;
}

// načte úvodní formulář
function loadWelcomeForm(){
	$("#main").load("pages/welcomeForm.html", function() {
        $(function(){
            $.getJSON("data/menu.json", function(data) {
                // insert main goal from JSON to the pop-up with information about the test      
                $("li[id^='mainGoal']").append(data.MainGoal);
                // build a html menu from json
                menuAccessKeys = buildList(data.Items, false);
                // creates a html menu without underlined access keys
                menu = menuAccessKeys.replace(/<u>|<\/u>|/gi, "");

                //TODO: menu without checkboxes! ---------------------------
                menuWithChck = buildListWithCheckbox(data.Items, false);

                $("#menu").html(menuAccessKeys); 
                $("#menu").hide();       
            });  
        })

    });
  
  // load menu data from json
  
}

// Uživatel o sobě vyplnil informace a spouští se test.
function startTest() {
    testerAge = $('#age').val();
    if($('#male').is(':checked')) {testerSex = "male"}
    if($('#female').is(':checked')) {testerSex = "female"}
    instructionCount = 0;
	shuffleMenus();
	getRandomInstruction();
	$("#menu").show();
}
