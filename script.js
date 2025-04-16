// Показываем рекламу через 1 секунду
setTimeout(showAd, 1000);

function showAd() {
	document.getElementById("adPopup").style.display = "block";
}

function closeAd() {
	document.getElementById("adPopup").style.display = "none";
}
