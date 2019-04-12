import { NgModule } from '@angular/core';
import { RouterModule, Routes } from '@angular/router';
import { LandingComponent } from './landing/landing.component';
import { PlatformComponent } from './platform/platform.component';
import { JobsComponent } from './platform/jobs/jobs.component';
import { JobComponent } from './platform/job/job.component';
import { ProfileComponent } from './platform/profile/profile.component';
import { RoutePath } from './shared/enums/route-path.enum';
import { AuthGuard } from './shared/guards/auth.guard';
import { RatingComponent } from './platform/rating/rating.component';
import { TestComponent } from './platform/test/test.component';

const routes: Routes = [
  {
    path: '',
    component: LandingComponent,
    canActivate: [AuthGuard],
  },
  {
    path: RoutePath.Platform,
    component: PlatformComponent,
    canActivate: [AuthGuard],
    children: [
      {
        path: RoutePath.Jobs, component: JobsComponent
      },
      {
        path: `${RoutePath.Jobs}/:id`, component: JobComponent
      },
      {
        path: `${RoutePath.Profile}`, component: ProfileComponent
      },
      {
        path: `${RoutePath.Rating}`, component: RatingComponent
      },
      {
        path: `${RoutePath.Test}`, component: TestComponent
      }
    ]
  },

];

@NgModule({
  imports: [RouterModule.forRoot(routes)],
  exports: [RouterModule]
})
export class AppRoutingModule {
}
