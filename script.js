let adClosed = false;
let adShown = false;

// Показываем рекламу через 1 секунду
setTimeout(showAd, 1000);

function showAd() {
	if (!adShown) {
		const adPopup = document.getElementById("adPopup");
		adPopup.style.display = "block";
		setTimeout(() => adPopup.classList.add('show'), 10);
		adShown = true;
	}
}

function closeAd() {
	const adPopup = document.getElementById("adPopup");
	adPopup.classList.remove('show');
	setTimeout(() => {
		adPopup.style.display = "none";
		adClosed = true;
	}, 500);
}

document.addEventListener('DOMContentLoaded', function () {
	const avatar = document.getElementById('avatar');
	const images = {
		a: new Image(),
		b: new Image()
	};
	images.a.src = 'assets/a.jpg';
	images.b.src = 'assets/b.avif';

	let currentImage = 'a';
	let isAnimating = false;

	avatar.addEventListener('mouseover', function () {
		if (!adClosed || isAnimating) return;

		isAnimating = true;
		avatar.style.transform = 'rotate(720deg)';

		const handleAnimationEnd = () => {
			avatar.removeEventListener('transitionend', handleAnimationEnd);
			currentImage = currentImage === 'a' ? 'b' : 'a';
			avatar.src = images[currentImage].src;
			avatar.style.transform = 'rotate(0deg)';
			isAnimating = false;
		};

		avatar.addEventListener('transitionend', handleAnimationEnd);
	});
});
