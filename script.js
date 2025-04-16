// Показываем рекламу через 1 секунду
setTimeout(showAd, 1000);

function showAd() {
	document.getElementById("adPopup").style.display = "block";
}

function closeAd() {
	document.getElementById("adPopup").style.display = "none";
}

document.addEventListener('DOMContentLoaded', function () {
	const avatar = document.getElementById('avatar');
	let isChanged = false;

	// Предзагрузка нового изображения
	const newImage = new Image();
	newImage.src = 'assets/b.avif'; // Укажите правильный путь

	avatar.addEventListener('mouseover', function () {
		if (isChanged) return;

		avatar.style.transform = 'rotate(720deg)';
		avatar.addEventListener('transitionend', function handler(e) {
			if (e.propertyName === 'transform') {
				avatar.removeEventListener('transitionend', handler);
				avatar.src = newImage.src;
				avatar.style.transform = 'none'; // Сброс вращения
				isChanged = true;
			}
		});
	});
});
