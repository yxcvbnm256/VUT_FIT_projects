import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import { PlatformComponent } from './platform.component';
import { ProfileComponent } from './profile/profile.component';
import { JobsComponent } from './jobs/jobs.component';
import { JobComponent } from './job/job.component';
import {
  MatCardModule,
  MatFormFieldModule,
  MatButtonModule, MatDatepickerModule,
  MatExpansionModule,
  MatIconModule, MatInputModule,
  MatListModule, MatNativeDateModule, MatSelectModule,
  MatSidenavModule, MatTabsModule,
  MatToolbarModule, MatCheckboxModule, MatSnackBarModule, MatSlideToggleModule, MatAutocompleteModule
} from '@angular/material';
import { RouterModule } from '@angular/router';
import { StarRatingModule } from 'angular-star-rating';
import { FlexLayoutModule } from '@angular/flex-layout';
import { NewJobComponent } from './new-job/new-job.component';
import { FormsModule, ReactiveFormsModule } from '@angular/forms';
import { CarouselModule } from 'angular2-carousel';
import { SafePipe } from '../shared/pipes/safe-style.pipe';
import { RatingComponent } from './rating/rating.component';
import { ImagePipe } from '../shared/pipes/image.pipe';
import { ImageUrlPipe } from '../shared/pipes/image-url.pipe';
import { TestComponent } from './test/test.component';
import { StatusIconPipe } from '../shared/pipes/status-icon.pipe';

@NgModule({
  imports: [
    CommonModule,
    RouterModule,
    StarRatingModule.forRoot(),
    FormsModule, ReactiveFormsModule,

    MatToolbarModule,
    MatIconModule,
    MatButtonModule,
    MatInputModule,
    MatSidenavModule,
    MatListModule,
    MatExpansionModule,
    MatTabsModule,
    MatFormFieldModule,
    MatCardModule,
    MatSelectModule,
    MatSidenavModule,
    MatDatepickerModule,
    MatNativeDateModule,
    MatCheckboxModule,
    MatSnackBarModule,
    MatSlideToggleModule,
    MatAutocompleteModule,

    FlexLayoutModule,
    CarouselModule,
  ],
  declarations: [
    PlatformComponent,
    ProfileComponent,
    JobsComponent,
    JobComponent,
    NewJobComponent,
    RatingComponent,
    SafePipe,
    ImagePipe,
    ImageUrlPipe,
    TestComponent,
    StatusIconPipe,
  ],
  providers: [
    SafePipe,
  ],
  entryComponents: []
})
export class PlatformModule {
}
