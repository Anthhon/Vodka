let blackTheme = false;

function flipTheme() { blackTheme = !blackTheme; }

function changeColorMode() {
    flipTheme();

    document.body.style.opacity = 0;
    document.body.style.scale = '90%';

    const secondaryColor = blackTheme ? '#191919' : '#fff';
    const primaryColor = blackTheme ? '#fff' : '#191919';
    const auxiliaryColor = blackTheme ? '#ffdd33' : '#3b33ff';

    setTimeout(() => {
        document.body.style.opacity = 100;
        document.body.style.scale = '100%';
    }, 250);

    document.documentElement.style.setProperty('--secondary-color', secondaryColor);
    document.documentElement.style.setProperty('--primary-color', primaryColor);
    document.documentElement.style.setProperty('--auxiliary-color', auxiliaryColor);
}
