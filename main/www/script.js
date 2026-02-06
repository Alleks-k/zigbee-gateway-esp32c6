function update(){
    fetch('/api/status').then(r=>r.json()).then(d=>{
        document.getElementById('status').innerHTML=`PAN ID: 0x${d.pan_id.toString(16)}, Channel: ${d.channel}, Short Addr: 0x${d.short_addr.toString(16)}`;
        let ul=document.getElementById('devices');ul.innerHTML='';d.devices.forEach(dev=>{let li=document.createElement('li');li.textContent=dev.name;ul.appendChild(li);});
    });
}
function permitJoin(){fetch('/api/permit_join',{method:'POST'}).then(r=>alert('Network opened'));}
setInterval(update,5000);update();