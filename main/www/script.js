function update(){
    fetch('/api/status').then(r=>r.json()).then(d=>{
        document.getElementById('status').innerHTML=`PAN ID: 0x${d.pan_id.toString(16)}, Channel: ${d.channel}, Short Addr: 0x${d.short_addr.toString(16)}`;
        let ul=document.getElementById('devices');ul.innerHTML='';d.devices.forEach(dev=>{
            let li=document.createElement('li');
            li.innerHTML=`${dev.name} <div class='device-controls'><button onclick='control(${dev.short_addr},1,1)'>On</button> <button onclick='control(${dev.short_addr},1,0)'>Off</button></div>`;
            ul.appendChild(li);
        });
    });
}
function permitJoin(){fetch('/api/permit_join',{method:'POST'}).then(r=>alert('Network opened'));}
function control(addr, endpoint, cmd) {
    fetch('/api/control', {
        method: 'POST',
        body: `${addr},${endpoint},${cmd}`
    }).then(r=>alert('Sent'));
}
setInterval(update,5000);update();