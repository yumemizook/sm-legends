local t = Def.ActorFrame{};

--explosion
t[#t+1] = Def.Sprite{
 	Texture=NOTESKIN:GetPath( 'Explosion', 'Spark' );
	InitCommand=cmd(blend,"BlendMode_Add";pause;diffusealpha,0);
	W1Command=cmd(playcommand,"NewExplosion");
	W2Command=cmd(playcommand,"NewExplosion");
	W3Command=cmd(playcommand,"NewExplosion");
	HoldingOnCommand=cmd(playcommand,"NewExplosion");
	-- HitMineCommand=cmd(playcommand,"NewExplosion");
	HeldCommand=cmd(playcommand,"NewExplosion");
	
	NewExplosionCommand=function(self)
		self:finishtweening();
		self:rotationz(0);
		self:aux(0);
		self:playcommand("Advance");
	end;

	AdvanceCommand=function(self)
		if self:getaux() < self:GetNumStates()-1 then
			self:queuecommand("Advance");
		else
			self:diffusealpha(0);
			return;
		end

		self:aux(self:getaux()+1);
		self:setstate(self:getaux());
		self:diffusealpha(1);
		self:linear(0.01);
	end;
};


t[#t+1] = Def.Sprite{
	Texture=NOTESKIN:GetPath( 'Explosion', 'Glow' );
	InitCommand=cmd(
		blend,"BlendMode_Add";
		diffusealpha,0;
	);
	W1Command=cmd(playcommand,"Glow");
	W2Command=cmd(playcommand,"Glow");
	W3Command=cmd(playcommand,"Glow");
	HoldingOnCommand=cmd(playcommand,"Glow");
	HeldCommand=cmd(playcommand,"Glow");
	GlowCommand=cmd(
		finishtweening;
		rotationz,0;
		zoom,.5;
		diffusealpha,1.5;
		decelerate,.3;
		diffusealpha,0;
		zoom,.35;
	);
};


return t;