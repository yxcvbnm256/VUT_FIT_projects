<?php
	$myFile = "results.json";
	$stringData = $_POST["data"];
	
	//$stringData = '{"menus":["menu/vertical/index.html","menu/compact/index.html","menu/dropdown/index.html"],"wrongClicks":[3,0,3],"resultTimes":[7892,1959,7476],"instructionsOrder":[6,2,3]}';
	//$stringData = '{"menus":["menu/vertical/index.html","menu/dropdown/index.html","menu/compact/index.html"],"wrognClicks":[0,0,0,0,1,0,1,1,2],"times":[213244,2914,3786,4655,5935,2916,4534,4951,2756],"instructions":[1,2,4,5,5,1,5,9,10]}';


	if (file_exists($myFile)) {
		$file = file_get_contents($myFile); // Load data from file
		$data = json_decode($file, true); // Decode so we can add more
		unset($file);//prevent memory leaks for large json.
		if ($data != NULL || $data != null) {
			$data[] = json_decode($stringData); // Add current data to json
			file_put_contents($myFile,json_encode($data)); // Encode to json and save
		}else{
			unset($data);
			$data = array();
			$data[0] = json_decode($stringData);
			file_put_contents($myFile,json_encode($data)); // Save just what you get
		}
	}else{
		$data = array();
		$data[0] = json_decode($stringData);
		file_put_contents($myFile,json_encode($data)); // Save just what you get
	}

	unset($data);//release memory
	unset($stringData);
	unset($myFile);

?>