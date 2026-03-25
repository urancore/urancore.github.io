const menuBtn = document.querySelector('.menu-button');
const sidebar = document.querySelector('.sidebar');

menuBtn.addEventListener('click', () => {
	sidebar.classList.toggle('active');
	const icon = menuBtn.querySelector('span');
	icon.textContent = sidebar.classList.contains('active') ? 'close' : 'menu';
});

document.addEventListener('click', (e) => {
	if (!menuBtn.contains(e.target) && !sidebar.contains(e.target)) {
		sidebar.classList.remove('active');
		menuBtn.querySelector('span').textContent = 'menu';
	}
});

window.addEventListener('resize', () => {
	if (window.innerWidth >= 768) {
		sidebar.classList.remove('active');
		menuBtn.querySelector('span').textContent = 'menu';
	}
});
