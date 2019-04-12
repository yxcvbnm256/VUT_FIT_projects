import { Injectable } from '@angular/core';
import { ActivatedRouteSnapshot, CanActivate, Router, RouterStateSnapshot } from '@angular/router';
import { Observable } from 'rxjs';
import { AngularFireAuth } from '@angular/fire/auth';
import { User } from 'firebase';
import { map, take } from 'rxjs/operators';
import { RoutePath } from '../enums/route-path.enum';

@Injectable({
  providedIn: 'root'
})
export class AuthGuard implements CanActivate {
  constructor(private readonly router: Router,
              private readonly fireAuth: AngularFireAuth) {
  }

  canActivate(
    next: ActivatedRouteSnapshot,
    state: RouterStateSnapshot): Observable<boolean> | Promise<boolean> | boolean {
    return this.fireAuth.authState.pipe(
      take(1),
      map((user: User) => {
        if (!user) {
          if (state.url === `/${RoutePath.Landing}`) {
            return true;
          }
          this.router.navigate([
            RoutePath.Landing
          ]);
          return false;
        }

        if (![RoutePath.Jobs, RoutePath.Profile, RoutePath.Rating, RoutePath.Test].some(item => state.url.includes(item))) {
          this.router.navigate([
            RoutePath.Platform,
            RoutePath.Jobs
          ]);
          return false;
        }
        return true;
      }));
  }
}
