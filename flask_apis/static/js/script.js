async function switch_button(button_id){

    const butt = document.getElementById(button_id)
    const butt_state = document.getElementById(button_id+"_status").innerText

    
    if (butt_state == "OFF"){
        await call_api(button_id, 1);
        document.getElementById(button_id+"_status").innerText = "ON"
        document.getElementById(button_id).style.backgroundColor = "rgb(39, 117, 190)"

    }
    
    if (butt_state == "ON"){
        await call_api(button_id, 0);
        document.getElementById(button_id+"_status").innerText = "OFF"
        document.getElementById(button_id).style.backgroundColor = "rgb(233, 233, 233)"

    }

}

async function call_api(pin,status){
    const data = {"pin":pin, "state":status}
    const url = "https://tmissacnewton.pythonanywhere.com/publish/"

    const response = await fetch(url, {
        method: 'PUT',
        headers: {
          'Content-type': 'application/json'
        },
        body: JSON.stringify(data)
      });
    const resData = await response.json();
 
    // Return response data
    console.log(resData);
    return resData;

}