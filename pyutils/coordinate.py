import pylab as pl

def locxyz2itrf(self, lat, longitude, alt, locx=0.0, locy=0.0, locz=0.0):
    """
    Returns the nominal ITRF (X, Y, Z) coordinates (m) for a point at "local"
    (x, y, z) (m) measured at geodetic latitude lat and longitude longitude
    (degrees) and altitude of the reference point of alt.  
    The ITRF frame used is not the official ITRF, just a right
    handed Cartesian system with X going through 0 latitude and 0 longitude,
    and Z going through the north pole.  The "local" (x, y, z) are measured
    relative to the closest point to (lat, longitude) on the WGS84 reference
    ellipsoid, with z normal to the ellipsoid and y pointing north.
    """
    # from Rob Reid;  need to generalize to use any datum...
    phi, lmbda = map(pl.radians, (lat, longitude))
    sphi = pl.sin(phi)
    a = 6378137.0      # WGS84 equatorial semimajor axis
    b = 6356752.3142   # WGS84 polar semimajor axis
    ae = pl.arccos(b / a)
    N = a / pl.sqrt(1.0 - (pl.sin(ae) * sphi)**2)
    
    # Now you see the connection between the Old Ones and Antarctica...
    Nploczcphimlocysphi = (N + locz+alt) * pl.cos(phi) - locy * sphi
    
    clmb = pl.cos(lmbda)
    slmb = pl.sin(lmbda)
    
    x = Nploczcphimlocysphi * clmb - locx * slmb
    y = Nploczcphimlocysphi * slmb + locx * clmb
    z = (N * (b / a)**2 + locz+alt) * sphi + locy * pl.cos(phi)
    
    return x, y, z
