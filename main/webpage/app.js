var network;
var password;
var interval;

window.onload = () => {
    document
        .querySelector("#show_password")
        .addEventListener("click", (element) => {
            console.log("Cehckbox checked");
            if (
                document.querySelector("#password").getAttribute("type") ===
                "password"
            ) {
                document
                    .querySelector("#password")
                    .setAttribute("type", "text");
            } else {
                document
                    .querySelector("#password")
                    .setAttribute("type", "password");
            }
        });

    document.querySelector("#Cancel").addEventListener("click", () => {
        var element = document.querySelector(".active");
        if (element != null) element.classList.remove("active");
        document.querySelector(".form2").style.display = "none";
        document.querySelector(".form1").style.display = "flex";
    });

    document.querySelector("#provision").addEventListener("click", () => {
        password = document.querySelector("#password").value;

        document.querySelector(".input").style.display = "none";
        document.querySelector(".result").style.display = "flex";

        $.ajax({
            url: "/provision",
            type: "GET",
            dataType: "json", // added data type
            headers: { ssid: network, pass: password },
            success: function (res) {
                provision_status();
                interval = setInterval(provision_status, 2500);
            },
        });
    });

    document.querySelector("#goto_form2").addEventListener("click", () => {
        var active_element = document.querySelector(".active");
        if (active_element === null) {
            alert("Selecione a rede pretendida");
            return;
        }

        network = active_element.innerHTML;
        document.querySelector("#wifi").innerHTML = network;
        document.querySelector(".form1").style.display = "none";
        document.querySelector(".form2").style.display = "flex";
    });

    scan_networks();
};

async function provision_status() {
    await $.ajax({
        url: "/status",
        type: "GET",
        dataType: "json",
        success: function (res) {
            var code = res.status;

            if (code == 0) {
                document.querySelector("#state").innerHTML =
                    "A conectar-se à rede";
            } else if (code == 1) {
                document.querySelector("#state").innerHTML = "Conectado";
                document.querySelector("#final").style.opacity = 1;
                clearInterval(interval);
            } else if (code == 2) {
                document.querySelector("#state").innerHTML = "Erro";
                clearInterval(interval);
            }
        },
    });
}

function scan_networks() {
    // Request the surrounding networks from the device
    $.ajax({
        url: "/scan_networks",
        type: "GET",
        dataType: "json", // added data type
        success: function (res) {
            document.querySelector(".redes").innerHTML = "";
            res.forEach((wifi) => {
                var wifi_node = document.createElement("p");
                wifi_node.innerHTML = wifi;
                document.querySelector(".redes").appendChild(wifi_node);
            });

            document
                .querySelector(".redes")
                .querySelectorAll("p")
                .forEach((element) => {
                    element.addEventListener("click", () => {
                        var active_element = document.querySelector(".active");
                        console.log(active_element);
                        if (active_element != null) {
                            active_element.classList.remove("active");
                        }
                        element.classList.add("active");
                    });
                });
        },
    });
}
