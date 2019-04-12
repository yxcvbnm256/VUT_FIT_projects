import { TestBed, inject } from '@angular/core/testing';

import { PwaService } from './pwa-service.service';

describe('PwaServiceService', () => {
  beforeEach(() => {
    TestBed.configureTestingModule({
      providers: [PwaService]
    });
  });

  it('should be created', inject([PwaService], (service: PwaService) => {
    expect(service).toBeTruthy();
  }));
});
