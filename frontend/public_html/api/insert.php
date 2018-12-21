<?php

include 'connect.php';
include 'dbutil.php';

$data = $_POST["data"];

connect();


	$name = $data['name'];
	$name=str_replace('"','\"',$name);


	$listname = $data['listname'];
	$listname=str_replace('"','\"',$listname);

	$date = $data['date'];
	$date=str_replace('"','\"',$date);
	$array =explode('.', $date);

	$date =$array[2]."-".$array[1]."-".$array[0];

	$country =$data['country'];
	$country=str_replace('"','\"',$country);

	$place =$data['place'];
	$place=str_replace('"','\"',$place);

	$accommodation=$data['accommodation'];
	$accommodation=str_replace('"','\"',$accommodation);

	$profession=$data['profession'];
	$profession=str_replace('"','\"',$profession);


	$note=$data['note'];
	$note=str_replace('"','\"',$note);



  	echo($name." ".$country." ".$place." ".$accommodation." ".$profession);


 	$list_id=idCheck('lists','name',$listname);
   	if($list_id==-1) $list_id=insertIntoDb('lists','name,time','"'.$listname.'","'.$date.'"');


// //  Neue Person einfügen und Id holen
  // $person_id=idCheck('persons','name',$name);
  //	if($person_id==-1)
   	$person_id=insertIntoDb('persons','name,note,profession','"'.$name.'","'.$note.'","'.$profession.'"');


// //  country_id holen
 	$country_id=idCheck('countries','name',$country);
  	if($country_id==-1)$country_id=insertIntoDb('countries','name','"'.$country.'"');


// //  location checken (<country_id>,<place>,<address>) nur wenns sie noch nicht gibt geolookup
 	$loc_id=locCheck('locations',$country_id,$place);
 	if($loc_id==-1){

	$addressstring = $country.",".$place;
 	$res=geoLookup($addressstring);
  	$lat=$res['lat'];
  	$lng=$res['lng'];
	$loc_id=insertIntoDb('locations','country_id,place,lat,lon','"'.$country_id.'","'.$place.'","'.$lat.'","'.$lng.'"');
	}

// 	//  Pesrons_locations
  	insertIntoDb('persons_locations','person_id,list_id,location_id','"'.$person_id.'","'.$list_id.'","'.$loc_id.'"');
 //	$person_id=valueCheck('persons_locations_list','person_id','name="'.$name.'" AND location_id="'.$loc_id.'"');

  	 	$array =explode(' ', $accommodation);
 		$street=$array[0];
 		$street_nr=$array[1];
 		$add=$array[2];

 	$ac_id=valueCheck('accommodations','id','historic_street="'.$street.'" AND historic_streetnr="'.$street_nr.'" AND historic_add="'.$add.'"');
	if($ac_id==-1){
		$country_id=idCheck('countries','name','Deutschland');
 		if($country_id==-1)$country_id=insertIntoDb('countries','name','"Deutschland"');



		$ac_id=insertIntoDb('accommodations','country_id,place,historic_street,historic_streetnr,historic_add','"'.$country_id.'","Bayreuth","'.$street.'","'.$street_nr.'","'.$add.'"');
	}

	 	insertIntoDb('persons_accommodations','person_id,list_id,accommodation_id','"'.$person_id.'","'.$list_id.'","'.$ac_id.'"');

  
// echo $theatre_id;

$mysqli->close();


////////////////////////////////////////////////


