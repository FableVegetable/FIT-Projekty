	function showHideDiv(div){
		var block = document.getElementById(div);
		block.style.display = (block.style.display=='none') ? 'block' : 'none';
		document.cookie = div + '=' + block.style.display + ';expires="";path=/';
	}
	
	function checkcookie(cookieid){
		var re=new RegExp(cookieid+'[^;]+','i');
		if (document.cookie.match(re)){
			document.getElementById(cookieid).style.display = document.cookie.match(re)[0].split("=")[1];
		}
	}
	
	function sethidden(div){
		var block = document.getElementById(div);
		block.style.display='none';
	}
	
	document.body.onload = sethidden('news1');
	document.body.onload = sethidden('news2');
	document.body.onload = sethidden('news3');
	document.body.onload = sethidden('showmorediv');
	document.body.onload = checkcookie('news1');
	document.body.onload = checkcookie('news2');
	document.body.onload = checkcookie('news3');
	document.body.onload = checkcookie('showmorediv');
	
	var slide = 0;
	var slides = document.getElementsByClassName("slides");
	var dots = document.getElementsByClassName("dot");
	
	showslide(slide, slides, dots);
	
	function nextslide(index) {showslide(slide += index, slides, dots);}
	function currentslide(index) {showslide(slide = index, slides, dots);}
	
	function showslide(n, slides, dots) {
		if (slide > slides.length) {slide = 1}    
		if (slide < 1) {slide = slides.length}
		for (var i = 0; i < slides.length; i++) {
			slides[i].style.display = "none";
			dots[i].className = dots[i].className.replace(" white-style", "");				 
		}
		slides[slide-1].style.display = "block";  
		dots[slide-1].className += " white-style";
	}
	
	slideshow();
	function slideshow() {
		var slides = document.getElementsByClassName("slides");
		var dots = document.getElementsByClassName("dot");
		for (var i = 0; i < slides.length; i++) {
			slides[i].style.display = "none";
			dots[i].className = dots[i].className.replace(" white-style", "");				  
		}
		if (++slide > slides.length) {slide = 1} 
		slides[slide-1].style.display = "block";
		dots[slide-1].className += " white-style";				
		setTimeout(slideshow, 4000); 
	}