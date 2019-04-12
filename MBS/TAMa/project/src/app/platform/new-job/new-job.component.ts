import { ChangeDetectionStrategy, Component, ElementRef, EventEmitter, Output, ViewChild } from '@angular/core';
import { FormBuilder, Validators } from '@angular/forms';
import { GeolocationService } from '../../services/geolocation.service';
import { GeocodingService } from '../../services/geocoding.service';
import { firestore } from 'firebase';
import { Job, JobStatus } from '../../models/job.model';
import { JobsService } from '../../services/jobs.service';
import { AngularFireAuth } from '@angular/fire/auth';
import { UserService } from '../../services/user.service';
import { CategoriesService } from '../../services/categories.service';
import { debounceTime, distinctUntilChanged, filter, switchMap } from 'rxjs/operators';
import { LocationIQAutocompleteItem } from '../../models/location-iqautocomplete';
import { Observable } from 'rxjs';


@Component({
  selector: 'tp-new-job',
  templateUrl: './new-job.component.html',
  styleUrls: ['./new-job.component.scss'],
  changeDetection: ChangeDetectionStrategy.OnPush,
})
export class NewJobComponent {

  group = this.builder.group({
    name: ['', Validators.required],
    description: ['', Validators.required],
    category: ['', Validators.required],
    dueTo: ['', Validators.required],
    address: ['', Validators.required],
    useCurrentLocation: [false],
  });

  location$ = this.geo.getCurrentPosition();
  images = [];

  categoriesList = {};

  @Output() submitted = new EventEmitter<any>();
  @ViewChild('inputFile') inputFile: ElementRef;

  filteredAddresses: Observable<LocationIQAutocompleteItem[]>;

  constructor(private builder: FormBuilder,
              private readonly geo: GeolocationService,
              private readonly geocode: GeocodingService,
              private readonly jobsService: JobsService,
              private readonly user: UserService,
              private readonly categories: CategoriesService) {
    this.categoriesList = this.categories.categories;

    const addressCtrl = this.group.get('address');
    this.user.getCurrentUserProfile().subscribe(
      profile => {
        if (profile !== undefined) {
          addressCtrl.setValue(profile.address);
        }
      }
    );
    this.group.get('useCurrentLocation').valueChanges.subscribe(
      value => {
        if (value) {
          addressCtrl.disable();
          addressCtrl.clearValidators();
          this.geo.getCurrentPosition().subscribe(
            position =>
              this.geocode.getReversedData(position.coords.latitude.toString(), position.coords.longitude.toString()).subscribe(
                data =>
                  addressCtrl.setValue(data.display_name)
              )
          );
        } else {
          addressCtrl.enable();
          addressCtrl.setValidators(Validators.required);
        }
      }
    );
    const debounce = 500;
    const minLength = 3;
    this.filteredAddresses = addressCtrl.valueChanges
      .pipe(
        debounceTime(debounce),
        distinctUntilChanged(),
        filter(val => val != null),
        filter(val => val.length >= minLength),
        switchMap(
          value =>
            this.geocode.autocomplete(value)
        )
      );
  }

  get categoriesKeys(): string[] {
    return Object.keys(this.categoriesList);
  }

  submit(): void {
    this.createNewJob();
    this.submitted.emit({
      status: 'complete',
      values: this.group.value,
    });
    this.group.reset();
  }

  cancel(): void {
    this.submitted.emit({
      status: 'canceled',
    });
    this.group.reset();
  }

  createNewJob(): void {
    const useCurrentlocation = this.group.get('useCurrentLocation').value;
    const values = this.group.value as Partial<Job>;

    if (useCurrentlocation) {
      this.geo.getCurrentPosition().subscribe(
        location => {
          const newJob = {
            ...values,
            applicants: [],
            location: new firestore.GeoPoint(location.coords.latitude, location.coords.longitude)
          };
          this.jobsService.insertNewJob(newJob, this.images);
        },
        err =>
          console.error(err)
      );
    } else {
      // TODO: info if it breaks
      const place = this.group.get('address').value;
      this.geocode.getData(place).subscribe(
        data => {
          const newJob = {
            ...values,
            status: JobStatus.PUBLISHED,
            applicants: [],
            location: new firestore.GeoPoint(+data.lat, +data.lon)
          };
          this.jobsService.insertNewJob(newJob, this.images);
        },
        err =>
          console.error(err)
      );
    }
  }

  upload(any): void {
    this.images = [...this.images, ...Array.from(this.inputFile.nativeElement.files)];
  }

  removePicture(index: number): void {
    this.images = this.images.filter((_, i) => i !== index);
  }
}
