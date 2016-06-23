// *******************************************************
// CS 174a Graphics Example Code
// animation.js - The main file and program start point.  The class definition here describes how to display an Animation and how it will react to key and mouse input.  Right now it has 
// very little in it - you will fill it in with all your shape drawing calls and any extra key / mouse controls.  

// Now go down to display() to see where the sample shapes are drawn, and to see where to fill in your own code.

"use strict"
var canvas, canvas_size, gl = null, g_addrs, set = false, ast_count = 70,
	movement = vec2(),	thrust = vec3(), 	looking = false, prev_time = 0, animate = false, animation_time = 0;
		var gouraud = false, color_normals = false, solid = false;

// end position of camera, don't change
var end_cor = translate(-27*Math.cos(5/3) + 3.5, 0, 23*Math.sin(5/3));

function CURRENT_BASIS_IS_WORTH_SHOWING(self, model_transform) { self.m_axis.draw( self.basis_id++, self.graphicsState, model_transform, new Material( vec4( .8,.3,.8,1 ), 1, 1, 1, 40, "" ) ); }


// *******************************************************	
// When the web page's window loads it creates an "Animation" object.  It registers itself as a displayable object to our other class "GL_Context" -- which OpenGL is told to call upon every time a
// draw / keyboard / mouse event happens.

window.onload = function init() {	var anim = new Animation();	}
function Animation()
{
	( function init (self) 
	{
		self.context = new GL_Context( "gl-canvas" );
		self.context.register_display_object( self );
		
		gl.clearColor( 0, 0, 0, 1 );			// Background color

		self.m_cube = new cube();
		self.m_axis = new axis();
		self.m_sphere = new sphere( mat4(), 4 );
		self.m_sphere3 = new sphere( mat4(), 3 );
		self.m_sphere2 = new sphere( mat4(), 2 );
		self.m_sphere5 = new sphere( mat4(), 5 );
		self.m_fan = new triangle_fan_full( 10, mat4() );
		self.m_strip = new rectangular_strip( 1, mat4() );
		self.m_cylinder = new cylindrical_strip( 150, mat4() );
		self.m_rocket = new rocket( mat4() );
		self.m_triangle_fan = new triangle_fan_full ( 20, mat4() );
		self.m_spaceship = new spaceship( mat4() );
		self.m_spaceship_stage2 = new spaceship_no_rocket( mat4() );
		self.m_ring1 = new ring( mat4(), 90, 5.8, 6.6 );
		self.m_ring2 = new ring( mat4(), 90, 6.7, 7.5 );
		self.m_ring3 = new ring( mat4(), 90, 7.7, 8.2 );
		// 1st parameter is camera matrix.  2nd parameter is the projection:  
		// The matrix that determines how depth is treated.  It projects 3D points onto a plane.
		self.graphicsState = new GraphicsState( lookAt(vec3(0, 250, 300), vec3(0,0,0), vec3(0,1,0)),  
			perspective(45, canvas.width/canvas.height, .1, 1000), 0 );

		// the transformation matrix for the spaceship
		self.spaceship_pos = mat4();
		self.comet_pos = mat4();
		self.fps;
		self.rocket_pos = mat4();
		
		self.eye_start = [ 0, 50, 200 ];
		self.eye = [ 0, 50, 200 ];

	//x_cor, x_ratio, y_ratio, frequency, self_freq, ast_scale, init_pos, deg, ast_mat 
		self.ast_x_cor = [];
		self.ast_x_ratio = [];
		self.ast_y_ratio = [];
		self.ast_y_cor = [];
		self.ast_freq = [];
		self.ast_self_freq = [];
		self.ast_scale = [];
		self.ast_init_pos = [];
		self.ast_deg = [];

		for (var i = 0; i < ast_count; i++) {
			self.ast_x_cor.push(Math.random() * (15 - 5.5) + 5.5);
			self.ast_x_ratio.push(Math.random() * (55 - 37) + 37);
			self.ast_y_ratio.push(Math.random() * (45 - 32) + 32);
			self.ast_y_cor.push(Math.random() * 2 - 1);
			self.ast_freq.push(Math.random() * (80000 - 10000) + 10000);
			self.ast_self_freq.push(Math.random() * (30 - 3) + 3);
			self.ast_scale.push(Math.random() * (0.23 - 0.08) + 0.08);
			self.ast_init_pos.push(Math.random() * 400000);
			self.ast_deg.push(Math.random() * 60 - 30);
		}
		// this.drawPlanet( 4    , 32, 28,  "mars",     20000,  20,   .6,  mars , 200000  );
		// this.drawPlanet( 15.5 , 60, 47,  "jupiter",  60000, 6,    4.5, jupiter , 53400 );



		gl.uniform1i( g_addrs.GOURAUD_loc, gouraud);		gl.uniform1i( g_addrs.COLOR_NORMALS_loc, color_normals);		gl.uniform1i( g_addrs.SOLID_loc, solid);
		
		self.context.render();	
	} ) ( this );	
	
	canvas.addEventListener('mousemove', function(e)	{		e = e || window.event;		movement = vec2( e.clientX - canvas.width/2, e.clientY - canvas.height/2, 0);	});
}

// *******************************************************	
// init_keys():  Define any extra keyboard shortcuts here
Animation.prototype.init_keys = function()
{
	shortcut.add( "Space", function() { thrust[1] = -1; } );			shortcut.add( "Space", function() { thrust[1] =  0; }, {'type':'keyup'} );
	shortcut.add( "z",     function() { thrust[1] =  1; } );			shortcut.add( "z",     function() { thrust[1] =  0; }, {'type':'keyup'} );
	shortcut.add( "w",     function() { thrust[2] =  1; } );			shortcut.add( "w",     function() { thrust[2] =  0; }, {'type':'keyup'} );
	shortcut.add( "a",     function() { thrust[0] =  1; } );			shortcut.add( "a",     function() { thrust[0] =  0; }, {'type':'keyup'} );
	shortcut.add( "s",     function() { thrust[2] = -1; } );			shortcut.add( "s",     function() { thrust[2] =  0; }, {'type':'keyup'} );
	shortcut.add( "d",     function() { thrust[0] = -1; } );			shortcut.add( "d",     function() { thrust[0] =  0; }, {'type':'keyup'} );
	shortcut.add( "f",     function() { looking = !looking; } );
	shortcut.add( ",",     ( function(self) { return function() { self.graphicsState.camera_transform = mult( rotate( 3, 0, 0,  1 ), self.graphicsState.camera_transform ); }; } ) (this) ) ;
	shortcut.add( ".",     ( function(self) { return function() { self.graphicsState.camera_transform = mult( rotate( 3, 0, 0, -1 ), self.graphicsState.camera_transform ); }; } ) (this) ) ;



	shortcut.add( "r",     ( function(self) { return function() { self.graphicsState.camera_transform = mat4(); }; } ) (this) );
	shortcut.add( "ALT+s", function() { solid = !solid;					gl.uniform1i( g_addrs.SOLID_loc, solid);	
																		gl.uniform4fv( g_addrs.SOLID_COLOR_loc, vec4(Math.random(), Math.random(), Math.random(), 1) );	 } );
	shortcut.add( "ALT+g", function() { gouraud = !gouraud;				gl.uniform1i( g_addrs.GOURAUD_loc, gouraud);	} );
	shortcut.add( "ALT+n", function() { color_normals = !color_normals;	gl.uniform1i( g_addrs.COLOR_NORMALS_loc, color_normals);	} );
	shortcut.add( "ALT+a", function() { animate = !animate; } );
	
	shortcut.add( "p",     ( function(self) { return function() { self.m_axis.basis_selection++; console.log("Selected Basis: " + self.m_axis.basis_selection ); }; } ) (this) );
	shortcut.add( "m",     ( function(self) { return function() { self.m_axis.basis_selection--; console.log("Selected Basis: " + self.m_axis.basis_selection ); }; } ) (this) );	
}

function update_camera( self, animation_delta_time, spaceship_pos )
	{

		var t = self.graphicsState.animation_time;
		if (t == 0 || !animate) {
			var leeway = 70, border = 50;
			var degrees_per_frame = .0005 * animation_delta_time;
			var meters_per_frame  = .03 * animation_delta_time;
																						// Determine camera rotation movement first
			var movement_plus  = [ movement[0] + leeway, movement[1] + leeway ];		// movement[] is mouse position relative to canvas center; leeway is a tolerance from the center.
			var movement_minus = [ movement[0] - leeway, movement[1] - leeway ];
			var outside_border = false;
			
			for( var i = 0; i < 2; i++ )
				if ( Math.abs( movement[i] ) > canvas_size[i]/2 - border )	outside_border = true;		// Stop steering if we're on the outer edge of the canvas.

			for( var i = 0; looking && outside_border == false && i < 2; i++ )			// Steer according to "movement" vector, but don't start increasing until outside a leeway window from the center.
			{
				var velocity = ( ( movement_minus[i] > 0 && movement_minus[i] ) || ( movement_plus[i] < 0 && movement_plus[i] ) ) * degrees_per_frame;	// Use movement's quantity unless the &&'s zero it out
				self.graphicsState.camera_transform = mult( rotate( velocity, i, 1-i, 0 ), self.graphicsState.camera_transform );			// On X step, rotate around Y axis, and vice versa.
			}
			self.graphicsState.camera_transform = mult( translate( scale_vec( meters_per_frame, thrust ) ), self.graphicsState.camera_transform );		// Now translation movement of camera, applied in local camera coordinate frame
			return;
		}
		// ELSE DO REAL TIME
		if (t < 5000) {
			self.graphicsState.camera_transform = 
				lookAt(vec3(0, 250-0.04*t, 300-0.02*t), vec3(0,0,0), vec3(0,1,0));
			// in the end of 5s, eye is at (0, 50, 200)
		} else if (t < 8000) {
			t = t - 5000;
			self.graphicsState.camera_transform = 
				lookAt(vec3(0, 50, 200), vec3(-t/3000 * 30,0,0), vec3(0,1,0));
		} else if (t < 11000) {
			t = t - 8000;
			self.graphicsState.camera_transform = 
				lookAt(vec3(0, 50, 200), vec3(-30 + t/3000 * 60,0,0), vec3(0,1,0));
		} else if (t < 14000) {
			t = t - 11000;
			self.graphicsState.camera_transform = 
				lookAt(vec3(0, 50, 200), vec3(30 - t/3000 * 30,0,0), vec3(0,1,0));
		} else if (t < 20000) {
			t = t - 14000;
			self.spaceship_pos = mult( translate(0, 1, 0), self.spaceship_pos );
			var dir = subtract( TransToCor(end_cor), self.eye_start );
			dir = scale_vec(t/7000, dir);
			self.eye = add(dir, self.eye_start);
			self.graphicsState.camera_transform = 
				lookAt( self.eye, TransToCor(self.spaceship_pos), vec3(0, 1, 0) );
		} else if (t < 35000) {
			t = t - 20000;
			var dir = subtract( TransToCor(self.spaceship_pos), self.eye );
			dir = scale_vec(t/30000, dir);
			self.eye = add(dir, self.eye);
			var at = TransToCor(self.spaceship_pos);
			at[1] += 0.25;
		 	var cur_eye = self.eye;
		 	cur_eye[1] += 0.03;
			self.graphicsState.camera_transform = 
				lookAt( cur_eye, at, vec3(0, 1, 0) );
		} 
		else if (t < 45000)
		 	self.graphicsState.camera_transform = lookAt(vec3(0,12,0), TransToCor(self.spaceship_pos), vec3(0,1,0));
		else if (t < 72000)
			self.graphicsState.camera_transform = lookAt(vec3(50, 15, 0), TransToCor(self.comet_pos), vec3(0,1,0));

	}



// *******************************************************	
// display(): called once per frame, whenever OpenGL decides it's time to redraw.

Animation.prototype.display = function(time)
	{
		if(!time) time = 0;
		this.animation_delta_time = time - prev_time;
		if(animate) this.graphicsState.animation_time += this.animation_delta_time;
		prev_time = time;



		update_camera( this, this.animation_delta_time );
		

		if (this.animation_delta_time != 0)
			this.fps = 1000/this.animation_delta_time;
			
		this.basis_id = 0;
		
		var model_transform = mat4();
		var sec_model_transform = mat4();
		var t = this.graphicsState.animation_time;
		var pi = Math.PI;

		var purplePlastic = new Material( vec4( .9,.5,.9,.7 ), 1, 1, 1, 40 ), // Omit the string parameter if you want no texture
			greyPlastic = new Material( vec4( .5,.5,.5,.7 ), 1, 1, .5, 20 ),
			stars = new Material( vec4( .5,.5,.5,1 ), 1, 1, 1, 40, "stars.png" ),
			sun = new Material( vec4( .5,.5,.5,0.2 ), 2.5, 2.5, 2.5, 60, "sun.png" ),
			sun_solid = new Material( vec4( .5,.5,.5,1 ), 2, 2, 2, 40, "sun.png" ),
			universe = new Material( vec4( .5,.5,.5,1 ), 1, 1, 1, 20, "universe.jpg" ),
			comet = new Material( vec4( .5,.5,.5,1 ), 1, 1, 1, 10, "halley.jpg" ),
			titanium = new Material( vec4( .5,.5,.5,1 ), 1, 1, 1, 10, "titanium.jpg" ),
			moon = new Material( vec4( .5,.5,.5,1 ), 1, 1, 1, 10, "moon.jpg" );

		var	earth = new Material( vec4( .5,.5,.5,1 ), 1, 1, .6, 5, "earth.jpg" ),
			venus = new Material( vec4( .5,.5,.5,1 ), 1, 1, .6, 10, "venus.jpg" ),
			mercury = new Material( vec4( .5,.5,.5,1 ), 1, 1, .6, 20, "mercury.png" ),
			mars = new Material( vec4( .5,.5,.5,1 ), 1, 1, .6, 5, "mars.jpg" ),
			jupiter = new Material( vec4( .5,.5,.5,1 ), 1, 1, .6, 3, "jupiter.jpg" ),
			saturn = new Material( vec4( .5,.5,.5,1 ), 1, 1, .6, 2, "saturn.png" ),
			uranus = new Material( vec4( .5,.5,.5,1 ), 1, 1, .6, 1, "uranus.jpg" ),
			neptune = new Material( vec4( .5,.5,.5,1 ), 1, 1, .6, 1, "neptune.jpg" );

		var stack = [];
		var sec_stack = [];

		// BACKGROUND: UNIVERSE
		stack.push(model_transform);
			model_transform = mult( model_transform, scale(600,600,600) );
			model_transform = mult(model_transform, rotate(-this.graphicsState.animation_time/700, 0, 1, 0));
			this.m_sphere.draw( this.graphicsState, model_transform, universe );
		model_transform = stack.pop();

		// SUN: overlap two suns together, use two transform matrix
		stack.push(model_transform);
		sec_stack.push(sec_model_transform);


		model_transform = mult(model_transform, scale(10, 10, 10));
		model_transform = mult(model_transform, rotate(-this.graphicsState.animation_time/100, 0, 1, 0));
		sec_model_transform = mult(sec_model_transform, 
												scale(10.2+.5*Math.abs(Math.cos(this.graphicsState.animation_time/1500)),
												10.2+.5*Math.abs(Math.cos(this.graphicsState.animation_time/1500)), 
												10.2+.5*Math.abs(Math.cos(this.graphicsState.animation_time/1500))));
		sec_model_transform = mult(sec_model_transform, rotate(this.graphicsState.animation_time/60, 0, 1, 0));

			this.m_sphere.draw( this.graphicsState, model_transform, sun_solid );
			this.m_sphere5.draw( this.graphicsState, sec_model_transform, sun );
		model_transform = stack.pop();
		sec_model_transform = sec_stack.pop();

		
	//==================================================
		this.drawPlanet( 2    , 17, 14,  "mercury",  2500,   3000, .4, mercury, 12000   );
		this.drawPlanet( 2.5  , 21, 19,  "venus",    6500,  1000, .9, venus  , 3000  );
		this.drawPlanet( 4    , 32, 28,  "mars",     20000,  20,   .6,  mars , 200000  );
		this.drawPlanet( 15.5 , 60, 47,  "jupiter",  60000, 6,    4.5, jupiter , 53400 );
		this.drawPlanet( 19.5 , 90, 75,  "saturn",   120000, 7,    3.7,   saturn  , 43040 );
		this.drawPlanet( 35.5 , 135, 120, "uranus",  500000, 15,   2.6,  uranus  , 920900 );
		this.drawPlanet( 50.5 , 175, 150, "neptune", 1000000, 16,   2.4,  neptune  , 6000 );
	
		this.drawComet(95, 120, 30, 18, 9000, .3, comet, 0); // halley

		for (var i = 0; i < ast_count; i++) {
		
			this.drawAstroids(
				this.ast_x_cor[i],
				this.ast_x_ratio[i],
				this.ast_y_ratio[i],
				this.ast_y_cor[i],
				this.ast_freq[i],
				this.ast_self_freq[i],
				this.ast_scale[i],
				this.ast_init_pos[i],
				this.ast_deg[i]
			);
		}
		//x_cor, x_ratio, y_ratio, y_cor, frequency, self_freq, ast_scale, init_pos, deg 

	//==================================================



	// EARTH ORBIT
	var x_cor = 3.5;
	var x_ratio = 27, y_ratio = 23;
	var planet_scale = 1;

	stack.push(model_transform);
		model_transform = mult(model_transform, translate(x_cor, 0, 0));
		model_transform = mult(model_transform, scale(x_ratio, 0.03, y_ratio));
		model_transform = mult(model_transform, rotate(90, 1, 0, 0));
			this.m_cylinder.draw( this.graphicsState, model_transform, greyPlastic );
	model_transform = stack.pop();
	
	// EARTH
		model_transform = mult(model_transform, translate(0, 0, y_ratio*Math.sin((this.graphicsState.animation_time )/12000)));
		model_transform = mult(model_transform, translate((0-x_ratio)*Math.cos((this.graphicsState.animation_time )/12000) + x_cor, 0, 0));
			// moon
				sec_stack.push(model_transform);
				model_transform = mult(model_transform, rotate(this.graphicsState.animation_time/50, 0, 1, 0));
				model_transform = mult(model_transform, translate(1.9, 0, 0));
				model_transform = mult(model_transform, scale(.2, .2, .2));
				this.m_sphere.draw( this.graphicsState, model_transform, moon );
				model_transform = sec_stack.pop();

			// spaceship!!!!!!!!!!!!
				sec_stack.push(model_transform);

				
				if (t < 22000) {
					//this.graphicsState.animation_time += 30000;
					// draw the regular spaceship
					model_transform = mult( model_transform, translate(0, .55, 0) );
					model_transform = mult( model_transform, scale(.1, .1, .1) );
					this.spaceship_pos = model_transform;	// store the trans matrix of spaceship for camera
					this.m_spaceship.draw( this.graphicsState, model_transform, titanium );

				} 
				else if (t < 30000 && t > 22000) {
					// launching
					var temp_t = t;
					temp_t -= 22000;

					model_transform = mult( model_transform, translate(0, .55, 0) );
					model_transform = mult( model_transform, scale(.1, .1, .1) );
					
					var trace = model_transform;
					model_transform = mult( translate(0, 0.04*Math.pow(temp_t/1000, 2), 0), model_transform);
					this.spaceship_pos = model_transform;
					for (var lag = 30; lag < 120; lag += 30) {
						var temp_trace = mult( // insert a lag constant
							translate( 0, 
									   0.4+0.04*Math.pow(Math.max((temp_t-lag)/1000, 0), 2),
									   0 ), 
							trace );

						this.drawTrace(temp_trace, 1.4, lag);
						this.drawTrace(temp_trace, -1.4, lag);
						this.drawTrace(temp_trace, 0, lag);
					}

					this.spaceship_pos = model_transform;	// store the trans matrix of spaceship for camera		
					this.m_spaceship.draw( this.graphicsState, model_transform, titanium );
				}

					// seperate the rocket and the spaceship
				else if (t < 35000 && t > 30000) {

					if (set == false) {
						this.rocket_pos = this.spaceship_pos;
						set = true;
					}

					var temp_t = t;
					temp_t -= 22000;

					model_transform = mult( model_transform, translate(0, .55, 0) );
					model_transform = mult( model_transform, scale(.1, .1, .1) );
					
					var trace = model_transform;
					model_transform = mult( translate(0, 0.04*Math.pow(temp_t/1000, 2), 0), model_transform);
					this.spaceship_pos = model_transform;
					for (var lag = 30; lag < 120; lag += 30) {
						var temp_trace = mult( // insert a lag constant
							translate( 0, 
									   0.4+0.04*Math.pow(Math.max((temp_t-lag)/1000, 0), 2),
									   0 ), 
							trace );
						this.drawTrace(temp_trace, 0, lag);
					}

					this.spaceship_pos = model_transform;	// store the trans matrix of spaceship for camera		
					this.m_spaceship_stage2.draw( this.graphicsState, model_transform, titanium );
					// rocket stays at the same position

					var rocket_pos = this.rocket_pos;
					rocket_pos = mult(rocket_pos, translate(1.3, 6, -3.25));
					rocket_pos = mult(rocket_pos, scale(.5,.5,.5));
					this.m_rocket.draw( this.graphicsState, rocket_pos, titanium );
					rocket_pos = this.rocket_pos;
					rocket_pos = mult(rocket_pos, translate(-1.3, 6, -3.25));
					rocket_pos = mult(rocket_pos, scale(.5,.5,.5));
					this.m_rocket.draw( this.graphicsState, rocket_pos, titanium );
				} 
				else if (t < 45000 && t >= 35000) {
					var t = this.graphicsState.animation_time - 35000;

					model_transform = mat4();

					model_transform = mult( model_transform, rotate(-90, 0, 1, 0 ) );
					model_transform = mult( model_transform, rotate(-90, 1, 0, 0 ) );
					
					
					model_transform = mult( translate( 13, 12, 11), model_transform );
					model_transform = mult( rotate(60*(t/10000), 0, 1, 0), model_transform);
					var trace = model_transform;

					model_transform = mult( translate(20 * t/25000, 15 * t/25000, -80 * t/25000), model_transform);

		

					this.m_spaceship_stage2.draw( this.graphicsState, model_transform, titanium );



					for (var lag = 60; lag < 260; lag += 50) {
						var temp_trace = mult( translate(20 * (t-lag)/25000, 15 * (t-lag)/25000, -80 * (t-lag)/25000), trace );
						this.drawTrace2(temp_trace, lag);
					}


					this.spaceship_pos = model_transform;
			
				}

				else if (t >= 72000)
					animate = false; // stop here

					
			

				 
				
				model_transform = sec_stack.pop();
			// REST OF THE PLANET: self rotating, scaling, axis tilt, etc.
		model_transform = mult(model_transform, scale(planet_scale, planet_scale, planet_scale));
		model_transform = mult(model_transform, rotate(25, 1,0,0));
		model_transform = mult(model_transform, rotate(this.graphicsState.animation_time/20, 0, 1, 0));
			this.m_sphere5.draw( this.graphicsState, model_transform, earth );
			// axis
		model_transform = mult(model_transform, scale(.01, 4, .01));
		model_transform = mult(model_transform, rotate(90, 1,0,0));
			this.m_cylinder.draw( this.graphicsState, model_transform, purplePlastic );
		

	}	


Animation.prototype.drawTrace2 = function ( trace_save, lag )
{
	var mat = new Material( vec4( 1, 0.54902, 0, .7 ), 1, 1, .5, 20 );
	var temp_t = this.graphicsState.animation_time;
// draw the trace when launching


	for (var i = 0; i < 14 - lag/300; i++) {
			var trace = trace_save;
				trace = mult(
							trace,
							translate(-.22, 4, -3.15) );
				trace = mult(
							trace,
							rotate(90, 0, 1, 0) );

				trace = mult(
							trace,
							scale(0.1, 0.1, 0.1));

				trace = mult(
							translate( 0.2*(2*Math.random()-1),
						    		   0.2*(2*Math.random()-1),
									   0.2*(2*Math.random()-1) ),
							trace );
				this.m_cube.draw(this.graphicsState, trace, mat);

	}	
}



Animation.prototype.drawTrace = function ( trace_save, pos, lag )
{
	var mat = new Material( vec4( 1, 0.54902, 0, .7 ), 1, 1, .5, 20 );
	var temp_t = this.graphicsState.animation_time;
// draw the trace when launching


	for (var i = 0; i < 13 - lag/200; i++) {
			var trace = trace_save;
				trace = mult(
							trace,
							translate(pos, 0, -3.15) );

				trace = mult(
							trace,
							scale(0.1*(1-lag/150), 0.1*(1-lag/150), 0.1*(1-lag/150)) );

				trace = mult(
							translate( 0.06*(1-lag/210)*(2*Math.random()-1),
						    		   0.03*(1-lag/210)*(2*Math.random()-1),
									   0.03*(1-lag/210)*(2*Math.random()-1) ),
							trace );
				this.m_cube.draw(this.graphicsState, trace, mat);

	}	
}



Animation.prototype.drawAstroids = function (x_cor, x_ratio, y_ratio, y_cor, frequency, self_freq, ast_scale, init_pos, deg ) 
{
	var model_transform = mat4();
	//var ast_mat = new Material( vec4( .5,.5,.5,1 ), 1, 1, 1, 10, "halley.jpg" );
	var ast_mat = new Material( vec4( .5,.5,.5,1 ), 1, 1, 1, 10, "halley.jpg" );
	model_transform = mult(model_transform, translate(0, y_cor, y_ratio*Math.sin((this.graphicsState.animation_time + init_pos)/frequency)));
	model_transform = mult(model_transform, translate((0-x_ratio)*Math.cos((this.graphicsState.animation_time + init_pos)/frequency) + x_cor, 0, 0));
			
	model_transform = mult(model_transform, scale(ast_scale, ast_scale, ast_scale));
	model_transform = mult(model_transform, rotate(deg, 0,0,1));
	model_transform = mult(model_transform, rotate(this.graphicsState.animation_time/self_freq, 0, 1, 0));
		this.m_sphere2.draw( this.graphicsState, model_transform, ast_mat);
}



Animation.prototype.drawComet = function ( x_cor, x_ratio, y_ratio, z_ratio, period, comet_scale, comet_mat, init_pos)
{
	var stack = [];
	var pi = Math.PI;
	var t = this.graphicsState.animation_time;
	var model_transform = mat4();
	var orbit_mat = new Material( vec4( .5,.5,.5,.6 ), 1, 1, .5, 20 );
	
	stack.push(model_transform);
		model_transform = mult(model_transform, translate(x_cor, -10, 0));
		model_transform = mult(model_transform, rotate(180/pi*-Math.asin(z_ratio/x_ratio), 0, 0, 1));
		model_transform = mult(model_transform, scale(x_ratio, comet_scale/5, y_ratio));
		model_transform = mult(model_transform, rotate(90, 1, 0, 0));
			this.m_cylinder.draw( this.graphicsState, model_transform, orbit_mat );
	model_transform = stack.pop();
	
	var trace = mat4(); // trace of comet
		model_transform = mult(model_transform, translate(x_cor, -10, 0));
		model_transform = mult(model_transform, rotate(180/pi*-Math.asin(z_ratio/x_ratio), 0, 0, 1));
			trace = model_transform;
		model_transform = mult(model_transform, 
					translate(0, 0, y_ratio*Math.sin((t+init_pos)/period)));
		model_transform = mult(model_transform, 
					translate((0-x_ratio)*Math.cos((t+init_pos)/period), 0, 0));
		model_transform = mult(model_transform, scale(comet_scale, comet_scale, comet_scale));
			this.m_sphere.draw( this.graphicsState, model_transform, comet_mat );
		this.comet_pos = model_transform;
	// draw trace
	var lag_max = 150;
	
	for (var lag = 30; lag <= lag_max; lag +=20 ) {
		for (var i = 0; i < 18 - lag/10; i++) {
			var t_trace = trace; // make copy of a temporary matrix
			t_trace = mult(t_trace, 
					translate(
					(0-x_ratio)*Math.cos((t+init_pos-lag)/period) + .25*(1-lag/200)*(2*Math.random()-1), 
					.25*(1-lag/200)*(2*Math.random()-1), 
					y_ratio*Math.sin((t+init_pos-lag)/period) + .25*(1-lag/200)*(2*Math.random()-1))
					);
			t_trace = mult(t_trace, scale(.08*(1-lag/200), .08*(1-lag/200), .08*(1-lag/200)));
			this.m_cube.draw( this.graphicsState, t_trace, orbit_mat );
		}
	}
}


// given a "center", draw a planet with sun at the focus of its orbits
// the orbit should also be represented in ellipse
// all planets are originally place along the x-axis
Animation.prototype.drawPlanet = function ( x_cor, x_ratio, y_ratio, name, frequency, self_freq, planet_scale, planet_mat, init_pos )
{
	var model_transform = mat4();
	var stack = [];
	var sec_stack = [];
	var orbit_mat = new Material( vec4( .5,.5,.5,.6 ), 1, 1, .5, 20 );
	var axis_mat = new Material( vec4( .9,.5,.9,.7 ), 1, 1, 1, 40 );
	// ORBIT of the planet
	stack.push(model_transform);
		model_transform = mult(model_transform, translate(x_cor, 0, 0));
		model_transform = mult(model_transform, scale(x_ratio, 0.03*planet_scale, y_ratio));
		model_transform = mult(model_transform, rotate(90, 1, 0, 0));
			this.m_cylinder.draw( this.graphicsState, model_transform, orbit_mat );
	model_transform = stack.pop();
	
	// PLANET
		model_transform = mult(model_transform, translate(0, 0, y_ratio*Math.sin((this.graphicsState.animation_time + init_pos)/frequency)));
		model_transform = mult(model_transform, translate((0-x_ratio)*Math.cos((this.graphicsState.animation_time + init_pos)/frequency) + x_cor, 0, 0));
			// REST OF THE PLANET: self rotating, scaling, axial tilt, etc.
		model_transform = mult(model_transform, scale(planet_scale, planet_scale, planet_scale));
		model_transform = mult(model_transform, rotate(-25, 0,0,1));
		model_transform = mult(model_transform, rotate(this.graphicsState.animation_time/self_freq, 0, 1, 0));
			this.m_sphere.draw( this.graphicsState, model_transform, planet_mat );

		model_transform = mult(model_transform, scale(1/planet_scale, 1/planet_scale, 1/planet_scale));
		model_transform = mult(model_transform, scale(.01*planet_scale, 4*planet_scale, .01*planet_scale));
		model_transform = mult(model_transform, rotate(90, 1,0,0));
			this.m_cylinder.draw( this.graphicsState, model_transform, axis_mat );
		if (name == "saturn") { // draw rings for saturn
			model_transform = mat4();
			model_transform = mult(model_transform, translate(0, 0, y_ratio*Math.sin((this.graphicsState.animation_time + init_pos)/frequency)));
			model_transform = mult(model_transform, translate((0-x_ratio)*Math.cos((this.graphicsState.animation_time + init_pos)/frequency) + x_cor, 0, 0));
			model_transform = mult(model_transform, rotate(-25, 0,0,1));
			model_transform = mult(model_transform, rotate(-90, 1,0,0));
			model_transform = mult(model_transform, rotate(-2*this.graphicsState.animation_time/self_freq, 0, 0, 1));
				this.m_ring1.draw( this.graphicsState, model_transform, planet_mat );
			model_transform = mult(model_transform, rotate(-1*this.graphicsState.animation_time/self_freq, 0, 0, 1));
				this.m_ring2.draw( this.graphicsState, model_transform, planet_mat );
			model_transform = mult(model_transform, rotate(-1*this.graphicsState.animation_time/self_freq, 0, 0, 1));
				this.m_ring3.draw( this.graphicsState, model_transform, planet_mat );

		}
}


Animation.prototype.update_strings = function( debug_screen_object )		// Strings this particular class contributes to the UI
{
	debug_screen_object.string_map["time"] = "Animation Time: " + this.graphicsState.animation_time/1000 + "s";
	//debug_screen_object.string_map["basis"] = "Showing basis: " + this.m_axis.basis_selection;
	debug_screen_object.string_map["animate"] = "Animation " + (animate ? "on" : "off") ;
	//debug_screen_object.string_map["test"] = "test: " + this.ast_freq[0];
	debug_screen_object.string_map["Fps"] = "Fps: " + this.fps;
}